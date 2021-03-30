#ifndef VEG_CORE_HPP_GJCBNFLAS
#define VEG_CORE_HPP_GJCBNFLAS

#include "veg/internal/macros.hpp"
#include "veg/internal/typedefs.hpp"
#include "veg/internal/integer_seq.hpp"
#include "veg/internal/prologue.hpp"

namespace veg {
inline namespace VEG_ABI {

namespace internal {
namespace meta_ {
template <bool B>
struct conditional_;
template <>
struct conditional_<true> {
	template <typename T, typename F>
	using type = T;
};
template <>
struct conditional_<false> {
	template <typename T, typename F>
	using type = F;
};
struct none {};
} // namespace meta_
} // namespace internal

namespace meta {

template <template <typename...> class F, typename... Ts>
struct meta_apply {
	using type = F<Ts...>;
};
template <typename T>
struct type_identity {
	using type = T;
};

template <bool B, typename T, typename F>
using conditional_t =
		typename internal::meta_::conditional_<B>::template type<T, F>;

template <typename T, T Value>
struct constant {
	static constexpr T value = Value;
};
template <typename T, T Value>
constexpr T constant<T, Value>::value;

template <bool B>
using bool_constant = constant<bool, B>;

using true_type = bool_constant<true>;
using false_type = bool_constant<false>;

template <typename Seq, typename... Bs>
struct and_test : false_type {};
template <typename Seq, typename... Bs>
struct or_test : true_type {};

template <usize Is, typename T>
using indexed = T;

template <typename... Ts>
struct pack_size {
	static constexpr usize value = sizeof...(Ts);
};
template <usize... Is>
struct and_test<index_sequence<Is...>, indexed<Is, true_type>...> : true_type {
};
template <usize... Is>
struct or_test<index_sequence<Is...>, indexed<Is, false_type>...> : false_type {
};

template <typename...>
using void_t = void;

template <typename... Preds>
struct disjunction;
template <typename... Preds>
struct conjunction;

template <>
struct disjunction<> : false_type {};
template <>
struct conjunction<> : true_type {};

template <typename First, typename... Preds>
struct disjunction<First, Preds...>
		: conditional_t<First::value, First, disjunction<Preds...>> {};

template <typename First, typename... Preds>
struct conjunction<First, Preds...>
		: conditional_t<First::value, conjunction<Preds...>, First> {};

} // namespace meta

namespace internal {
namespace meta_ {

template <typename T>
struct decay_helper : meta::type_identity<T> {};
template <typename Ret, typename... Args>
struct decay_helper<Ret(Args...)> : meta::type_identity<Ret (*)(Args...)> {};
template <typename T, usize N>
struct decay_helper<T[N]> : meta::type_identity<T*> {};

using namespace meta;

#if __cplusplus >= 201402L

template <typename Enable, template <typename...> class Ftor, typename... Args>
constexpr bool _is_detected_v = false;
template <template <typename...> class Ftor, typename... Args>
constexpr bool _is_detected_v<meta::void_t<Ftor<Args...>>, Ftor, Args...> =
		true;

template <template <typename...> class Op, typename... Args>
constexpr bool is_detected_v = _is_detected_v<void, Op, Args...>;

#else

template <typename Enable, template <typename...> class Ftor, typename... Args>
struct _detector {
	using value_type = false_type;
};
template <template <typename...> class Ftor, typename... Args>
struct _detector<meta::void_t<Ftor<Args...>>, Ftor, Args...> {
	using value_type = true_type;
};

template <typename Default, template <typename...> class Ftor, typename... Args>
struct detector : _detector<void, Ftor, Args...> {};

template <template <typename...> class Op, typename... Args>
using is_detected = typename detector<meta_::none, Op, Args...>::value_type;

#endif

template <typename T>
struct is_pointer : false_type, type_identity<T> {};
template <typename T>
struct is_pointer<T*> : true_type, type_identity<T> {};

struct wrapper_base {
	static auto test(...) -> false_type;
};
template <typename T>
struct wrapper : wrapper_base {
	using wrapper_base::test;
	static auto test(wrapper<T>*) -> true_type;
};

template <typename Base>
struct baseof_wrapper : wrapper_base {
	using wrapper_base::test;
	static auto test(Base const volatile*) -> true_type;
};

template <typename T, typename U>
using is_same = decltype(wrapper<T>::test(static_cast<wrapper<U>*>(nullptr)));
template <typename Base, typename Derived>
using is_base_of =
		decltype(baseof_wrapper<Base>::test(static_cast<Derived>(nullptr)));

template <typename T>
struct is_lvalue_reference : false_type {};
template <typename T>
struct is_lvalue_reference<T&> : true_type {};
template <typename T>
struct is_rvalue_reference : false_type {};
template <typename T>
struct is_rvalue_reference<T&&> : true_type {};

template <typename T>
struct is_const : false_type {};
template <typename T>
struct is_const<T const> : true_type {};

} // namespace meta_
} // namespace internal

namespace concepts {
using meta::conjunction;
using meta::disjunction;

#define VEG_DEF_CONCEPT_BUILTIN_OR_INTERNAL(Tpl, Name, ...)                    \
	VEG_DEF_CONCEPT(                                                             \
			Tpl,                                                                     \
			Name,                                                                    \
			VEG_HAS_BUILTIN_OR(                                                      \
					__is_##Name,                                                         \
					__is_##Name(__VA_ARGS__),                                            \
					(internal::meta_::is_##Name<__VA_ARGS__>::value)))

#if __cplusplus >= 202002L
VEG_DEF_CONCEPT(
		(template <typename...> class Op, typename... Args), detected, requires {
			typename Op<Args...>;
		});
#elif __cplusplus >= 201402L
VEG_DEF_CONCEPT(
		(template <typename...> class Op, typename... Args),
		detected,
		internal::meta_::is_detected_v<Op, Args...>);
#else
VEG_DEF_CONCEPT(
		(template <typename...> class Op, typename... Args),
		detected,
		internal::meta_::is_detected<Op, Args...>::value);
#endif

VEG_DEF_CONCEPT_BUILTIN_OR_INTERNAL((typename T, typename U), same, T, U);
VEG_DEF_CONCEPT_BUILTIN_OR_INTERNAL((typename T, typename U), base_of, T, U);

VEG_DEF_CONCEPT(
		typename T,
		const_type,
		VEG_HAS_BUILTIN_OR(
				__is_const, __is_const(T), (internal::meta_::is_const<T>::value)));
VEG_DEF_CONCEPT(
		typename T,
		void_type,
		VEG_CONCEPT(same<void const volatile, T const volatile>));
VEG_DEF_CONCEPT(typename T, complete, (sizeof(T) == sizeof(T)));

// can't use __is_pointer because of <bits/cpp_type_traits.h> header
VEG_DEF_CONCEPT(typename T, pointer, internal::meta_::is_pointer<T>::value);

VEG_DEF_CONCEPT_BUILTIN_OR_INTERNAL(typename T, lvalue_reference, T);
VEG_DEF_CONCEPT_BUILTIN_OR_INTERNAL(typename T, rvalue_reference, T);
VEG_DEF_CONCEPT_DISJUNCTION(
		typename T, reference, ((, lvalue_reference<T>), (, rvalue_reference<T>)));

} // namespace concepts

namespace meta {
template <typename T>
using unref_t = typename internal::meta_::unref<T&>::type;
template <typename T>
using unptr_t = typename internal::meta_::is_pointer<T>::type;

template <typename Default, template <typename...> class Op, typename... Args>
using detected_or_t = typename meta::conditional_t<
		VEG_CONCEPT(detected<Op, Args...>),
		meta::meta_apply<Op, Args...>,
		meta::type_identity<Default>>::type;

template <template <typename...> class Op, typename... Args>
using detected_t = detected_or_t<internal::meta_::none, Op, Args...>;

template <typename T>
using decay_t = typename internal::meta_::decay_helper<unref_t<T>>::type;
} // namespace meta
} // namespace VEG_ABI
} // namespace veg

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_CORE_HPP_GJCBNFLAS */
