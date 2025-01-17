#ifndef VEG_MACROS_HPP_HSTLSKZXS
#define VEG_MACROS_HPP_HSTLSKZXS
#include "veg/internal/.external/hedley.ext.h"
#include "veg/internal/typedefs.hpp"
#include "veg/internal/preprocessor.hpp"
#include "veg/internal/prologue.hpp"
#include <initializer_list>

////////////////////////////////////////////////////////////////////////////////

#if HEDLEY_MSVC_VERSION_CHECK(14, 0, 0) ||                                     \
		HEDLEY_INTEL_CL_VERSION_CHECK(2021, 1, 0)
#define VEG_DEPRECATED(Reason) __declspec(deprecated(Reason))
#elif HEDLEY_HAS_EXTENSION(attribute_deprecated_with_message) ||               \
		HEDLEY_GCC_VERSION_CHECK(4, 5, 0) ||                                       \
		HEDLEY_INTEL_VERSION_CHECK(13, 0, 0) ||                                    \
		HEDLEY_ARM_VERSION_CHECK(5, 6, 0) ||                                       \
		HEDLEY_SUNPRO_VERSION_CHECK(5, 13, 0) ||                                   \
		HEDLEY_PGI_VERSION_CHECK(17, 10, 0) ||                                     \
		HEDLEY_TI_VERSION_CHECK(18, 1, 0) ||                                       \
		HEDLEY_TI_ARMCL_VERSION_CHECK(18, 1, 0) ||                                 \
		HEDLEY_TI_CL6X_VERSION_CHECK(8, 3, 0) ||                                   \
		HEDLEY_TI_CL7X_VERSION_CHECK(1, 2, 0) ||                                   \
		HEDLEY_TI_CLPRU_VERSION_CHECK(2, 3, 0)
#define VEG_DEPRECATED(Reason) __attribute__((__deprecated__(Reason)))
#elif defined(__cplusplus) && (__cplusplus >= 201402L)
#define VEG_DEPRECATED(Reason)                                                 \
	HEDLEY_DIAGNOSTIC_DISABLE_CPP98_COMPAT_WRAP_([[deprecated(Reason)]])
#elif HEDLEY_HAS_ATTRIBUTE(deprecated) || HEDLEY_GCC_VERSION_CHECK(3, 1, 0) || \
		HEDLEY_ARM_VERSION_CHECK(4, 1, 0) || HEDLEY_TI_VERSION_CHECK(15, 12, 0) || \
		(HEDLEY_TI_ARMCL_VERSION_CHECK(4, 8, 0) &&                                 \
     defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) ||                                 \
		HEDLEY_TI_ARMCL_VERSION_CHECK(5, 2, 0) ||                                  \
		(HEDLEY_TI_CL2000_VERSION_CHECK(6, 0, 0) &&                                \
     defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) ||                                 \
		HEDLEY_TI_CL2000_VERSION_CHECK(6, 4, 0) ||                                 \
		(HEDLEY_TI_CL430_VERSION_CHECK(4, 0, 0) &&                                 \
     defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) ||                                 \
		HEDLEY_TI_CL430_VERSION_CHECK(4, 3, 0) ||                                  \
		(HEDLEY_TI_CL6X_VERSION_CHECK(7, 2, 0) &&                                  \
     defined(__TI_GNU_ATTRIBUTE_SUPPORT__)) ||                                 \
		HEDLEY_TI_CL6X_VERSION_CHECK(7, 5, 0) ||                                   \
		HEDLEY_TI_CL7X_VERSION_CHECK(1, 2, 0) ||                                   \
		HEDLEY_TI_CLPRU_VERSION_CHECK(2, 1, 0)
#define VEG_DEPRECATED(Reason) __attribute__((__deprecated__))
#elif HEDLEY_MSVC_VERSION_CHECK(13, 10, 0) ||                                  \
		HEDLEY_PELLES_VERSION_CHECK(6, 50, 0) ||                                   \
		HEDLEY_INTEL_CL_VERSION_CHECK(2021, 1, 0)
#define VEG_DEPRECATED(Reason) __declspec(deprecated)
#elif HEDLEY_IAR_VERSION_CHECK(8, 0, 0)
#define VEG_DEPRECATED(Reason) _Pragma("deprecated")
#else
#define VEG_DEPRECATED(Reason)
#endif

////////////////////////////////////////////////////////////////////////////////

#if defined(__clang__)
#define VEG_WRAP_SILENCE_WARNING(...)                                          \
	HEDLEY_DIAGNOSTIC_PUSH _Pragma(                                              \
			"clang diagnostic ignored \"-Wc++17-extensions\"")                       \
			__VA_ARGS__ HEDLEY_DIAGNOSTIC_POP
#else
#define VEG_WRAP_SILENCE_WARNING(...) __VA_ARGS__
#endif

#ifndef VEG_HAS_NO_UNIQUE_ADDRESS
#define VEG_HAS_NO_UNIQUE_ADDRESS 0
#endif

#if VEG_HAS_NO_UNIQUE_ADDRESS
#ifdef _MSC_VER
#define VEG_NO_UNIQUE_ADDRESS [[msvc::no_unique_address]]
#else
#define VEG_NO_UNIQUE_ADDRESS [[no_unique_address]]
#endif
#else
#define VEG_NO_UNIQUE_ADDRESS
#endif

#ifndef VEG_INLINE
#define VEG_INLINE HEDLEY_ALWAYS_INLINE
#endif
#define VEG_NO_INLINE HEDLEY_NEVER_INLINE

#if defined(__cpp_concepts) && __cpp_concepts >= 201907L
#define VEG_HAS_CONCEPTS 1
#else
#define VEG_HAS_CONCEPTS 0
#endif

#if __cplusplus >= 201703L
#define VEG_DECLVAL(...) (static_cast<__VA_ARGS__ (*)() noexcept>(nullptr)())
#else
#define VEG_DECLVAL(...) (::veg::_detail::_meta::declval<__VA_ARGS__>())
#endif

#if defined(__clang__)
#define VEG_ARROW(...)                                                         \
	__attribute__((always_inline)) noexcept(noexcept((__VA_ARGS__)))             \
			->decltype((__VA_ARGS__)) {                                              \
		return __VA_ARGS__;                                                        \
	}
#elif defined(__GNUC__) && (__GNUC__ >= 9)
#define VEG_ARROW(...)                                                         \
	noexcept(noexcept((__VA_ARGS__))) __attribute__((always_inline))             \
			->decltype((__VA_ARGS__)) {                                              \
		return __VA_ARGS__;                                                        \
	}
#else
#define VEG_ARROW(...)                                                         \
	noexcept(noexcept((__VA_ARGS__)))->decltype((__VA_ARGS__)) {                 \
		return __VA_ARGS__;                                                        \
	}
#endif

#define VEG_LAZY_BY_REF(...) [&]() VEG_ARROW(__VA_ARGS__)

#define VEG_LIFT(...)                                                          \
	[&](auto&&... args) VEG_ARROW((__VA_ARGS__)(VEG_FWD(args)...))

#define VEG_DEDUCE_RET(...)                                                    \
	noexcept(noexcept(__VA_ARGS__))->decltype(__VA_ARGS__) {                     \
		return __VA_ARGS__;                                                        \
	}                                                                            \
	VEG_NOM_SEMICOLON

#if __cplusplus >= 201703L
#define VEG_HAS_FOLD_EXPR 1
#define VEG_ALL_OF(...) (__VA_ARGS__ && ... && true)
#define VEG_ANY_OF(...) (__VA_ARGS__ || ... || false)
#elif defined(__clang__)
#define VEG_HAS_FOLD_EXPR 1
#define VEG_ALL_OF(...) VEG_WRAP_SILENCE_WARNING((__VA_ARGS__ && ... && true))
#define VEG_ANY_OF(...) VEG_WRAP_SILENCE_WARNING((__VA_ARGS__ || ... || false))
#else
#define VEG_HAS_FOLD_EXPR 0
#define VEG_ALL_OF(...)                                                        \
	::veg::meta::and_test<                                                       \
			::veg::meta::make_index_sequence<                                        \
					::veg::meta::pack_size<decltype((void)(__VA_ARGS__))...>::value>,    \
			::veg::meta::bool_constant<(__VA_ARGS__)>...>::value
#define VEG_ANY_OF(...)                                                        \
	::veg::meta::or_test<                                                        \
			::veg::meta::make_index_sequence<                                        \
					::veg::meta::pack_size<decltype((void)(__VA_ARGS__))...>::value>,    \
			::veg::meta::bool_constant<(__VA_ARGS__)>...>::value
#endif

#define VEG_EVAL_ALL(...)                                                      \
	((void)(::veg::_detail::EmptyArr{                                            \
			::veg::_detail::Empty{},                                                 \
			((void)(__VA_ARGS__), ::veg::_detail::Empty{})...}))

////////////////////////////////////////////////////////////////////////////////

#if VEG_HAS_CONCEPTS

#define __VEG_IMPL_AND(_, Param) &&__VEG_PP_UNWRAP(Param)
#define __VEG_IMPL_OR(_, Param) || __VEG_PP_UNWRAP(Param)

#define __VEG_IMPL_CONJUNCTION(First, ...)                                     \
	(__VEG_PP_UNWRAP(First)                                                      \
	     __VEG_PP_TUPLE_FOR_EACH(__VEG_IMPL_AND, _, (__VA_ARGS__)))

#define __VEG_IMPL_DISJUNCTION(First, ...)                                     \
	(__VEG_PP_UNWRAP(First)                                                      \
	     __VEG_PP_TUPLE_FOR_EACH(__VEG_IMPL_OR, _, (__VA_ARGS__)))

#define VEG_DEF_CONCEPT(Tpl, Name, ...)                                        \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	concept Name = __VA_ARGS__;                                                  \
	VEG_NOM_SEMICOLON

#define VEG_CHECK_CONCEPT_MACRO(Namespace, ...)                                \
	static_assert(                                                               \
			Namespace::__VA_ARGS__, __VEG_PP_STRINGIZE(__VA_ARGS__) " failed")
#define VEG_CONCEPT_MACRO(Namespace, ...) Namespace::__VA_ARGS__

#define VEG_DEF_CONCEPT_CONJUNCTION(Tpl, Name, Terms)                          \
	VEG_DEF_CONCEPT(Tpl, Name, __VEG_IMPL_CONJUNCTION Terms)
#define VEG_DEF_CONCEPT_DISJUNCTION(Tpl, Name, Terms)                          \
	VEG_DEF_CONCEPT(Tpl, Name, __VEG_IMPL_DISJUNCTION Terms)

#define VEG_CONCEPT_EXPR(Tpl, TplNames, Name, Expr, ...)                       \
	namespace _veg_detail {                                                      \
	template <typename ExprType, __VEG_PP_REMOVE_PAREN1(Tpl)>                    \
	concept test_return_##Name = __VA_ARGS__;                                    \
	}                                                                            \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	concept Name = _veg_detail::                                                 \
			test_return_##Name<decltype((Expr)), __VEG_PP_REMOVE_PAREN1(TplNames)>;  \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	concept xnothrow_##Name = noexcept(Expr);                                    \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	concept nothrow_##Name = noexcept(Expr);                                     \
	VEG_NOM_SEMICOLON

#else

#if __cplusplus >= 201402L
#define __VEG_IMPL_DEF_CONCEPT(Tpl, Name, Value, ...)                          \
	namespace _ {                                                                \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	struct Name : __VA_ARGS__ {};                                                \
	}                                                                            \
	namespace {                                                                  \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	constexpr bool const& Name = ::veg::meta::bool_constant<Value>::value;       \
	}                                                                            \
	VEG_NOM_SEMICOLON
#else
#define __VEG_IMPL_DEF_CONCEPT(Tpl, Name, Value, ...)                          \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	struct Name : __VA_ARGS__ {}
#endif

#ifdef __clang__
#define __VEG_NO_WARNING_PRAGMA_PUSH                                           \
	HEDLEY_DIAGNOSTIC_PUSH _Pragma("clang diagnostic ignored \"-Wconversion\"")
#define __VEG_NO_WARNING_PRAGMA_POP HEDLEY_DIAGNOSTIC_POP
#else
#define __VEG_NO_WARNING_PRAGMA_PUSH
#define __VEG_NO_WARNING_PRAGMA_POP
#endif

#define VEG_CONCEPT_EXPR(Tpl, TplNames, Name, Expr, ...)                       \
	namespace _veg_detail {                                                      \
	template <typename _veg_Enable, __VEG_PP_REMOVE_PAREN1(Tpl)>                 \
	struct test_sfinae_##Name {                                                  \
		using TestExpr = ::veg::meta::false_type;                                  \
		using NothrowTestExpr = ::veg::meta::false_type;                           \
	};                                                                           \
	template <__VEG_PP_REMOVE_PAREN1(Tpl)>                                       \
	struct test_sfinae_##Name<                                                   \
			::veg::meta::void_t<decltype((Expr))>,                                   \
			__VEG_PP_REMOVE_PAREN1(TplNames)> {                                      \
		using ExprType = decltype((Expr));                                         \
		using TestExpr = ::veg::meta::bool_constant<__VA_ARGS__>;                  \
		using NothrowTestExpr = ::veg::meta::bool_constant<                        \
				(TestExpr::value) && __VEG_NO_WARNING_PRAGMA_PUSH noexcept(Expr)       \
																 __VEG_NO_WARNING_PRAGMA_POP>;                 \
	};                                                                           \
	}                                                                            \
	VEG_DEF_CONCEPT(                                                             \
			Tpl,                                                                     \
			Name,                                                                    \
			_veg_detail::test_sfinae_##Name<                                         \
					void,                                                                \
					__VEG_PP_REMOVE_PAREN1(TplNames)>::TestExpr::value);                 \
	VEG_DEF_CONCEPT(Tpl, nothrow_##Name, noexcept(Expr));                        \
	VEG_DEF_CONCEPT(                                                             \
			Tpl,                                                                     \
			xnothrow_##Name,                                                         \
			_veg_detail::test_sfinae_##Name<                                         \
					void,                                                                \
					__VEG_PP_REMOVE_PAREN1(TplNames)>::NothrowTestExpr::value);          \
	VEG_NOM_SEMICOLON

#if __cplusplus >= 201402L
#define VEG_CONCEPT_MACRO(Namespace, ...) Namespace::__VA_ARGS__
#define __VEG_IMPL_ADD_VALUE(I, _, Param) (Param)
#define __VEG_IMPL_TRAIT(Param) __VEG_PP_HEAD Param _::__VEG_PP_TAIL Param
#else
#define VEG_CONCEPT_MACRO(Namespace, ...) Namespace::__VA_ARGS__::value
#define __VEG_IMPL_ADD_VALUE(I, _, Param)                                      \
	((__VEG_PP_REMOVE_PAREN(Param)::value))
#define __VEG_IMPL_TRAIT(Param) __VEG_PP_UNWRAP(Param)
#endif
#define __VEG_IMPL_PUT_TRAIT(I, _, Param) __VEG_IMPL_TRAIT(Param)

#define VEG_CHECK_CONCEPT_MACRO(Namespace, ...)                                \
	static_assert(                                                               \
			decltype(Namespace::check_##__VA_ARGS__())::value,                       \
			__VEG_PP_STRINGIZE(__VA_ARGS__) " failed")
#define VEG_DEF_CONCEPT(Tpl, Name, ...)                                        \
	__VEG_IMPL_DEF_CONCEPT(                                                      \
			Tpl, Name, (__VA_ARGS__), ::veg::meta::bool_constant<__VA_ARGS__>);      \
	VEG_TEMPLATE(                                                                \
			Tpl, requires(__VA_ARGS__), constexpr auto check_##Name, (_ = 0, int))   \
	noexcept->::veg::meta::true_type

#define __VEG_IMPL_SFINAE(_, Param)                                            \
	, ::veg::meta::enable_if_t<__VEG_PP_UNWRAP Param, int> = 0

#define __VEG_IMPL_OVERLOAD(Name_Tpl, Param)                                   \
	template <                                                                   \
			__VEG_PP_REMOVE_PAREN(__VEG_PP_TAIL Name_Tpl),                           \
			::veg::meta::enable_if_t<__VEG_PP_UNWRAP Param, int> = 0>                \
	auto __VEG_PP_CAT(                                                           \
			check_, __VEG_PP_HEAD Name_Tpl)() noexcept->::veg::meta::true_type;

#define VEG_DEF_CONCEPT_BOOL_CONJUNCTION_IMPL(Tpl, Name, Base, Seq)            \
	__VEG_IMPL_DEF_CONCEPT(                                                      \
			Tpl,                                                                     \
			Name,                                                                    \
			(__VEG_PP_REMOVE_PAREN1(Base)::value),                                   \
			__VEG_PP_REMOVE_PAREN1(Base));                                           \
	template <__VEG_PP_REMOVE_PAREN(Tpl)                                         \
	              __VEG_PP_TUPLE_FOR_EACH(__VEG_IMPL_SFINAE, _, Seq)>            \
	auto check_##Name() noexcept->::veg::meta::true_type
#define VEG_DEF_CONCEPT_BOOL_DISJUNCTION_IMPL(Tpl, Name, Base, Seq)            \
	__VEG_IMPL_DEF_CONCEPT(                                                      \
			Tpl,                                                                     \
			Name,                                                                    \
			(__VEG_PP_REMOVE_PAREN1(Base)::value),                                   \
			__VEG_PP_REMOVE_PAREN1(Base));                                           \
	__VEG_PP_TUPLE_FOR_EACH(__VEG_IMPL_OVERLOAD, (Name, Tpl), Seq)               \
	VEG_NOM_SEMICOLON

#define VEG_DEF_CONCEPT_CONJUNCTION(Tpl, Name, Terms)                          \
	VEG_DEF_CONCEPT_BOOL_CONJUNCTION_IMPL(                                       \
			Tpl,                                                                     \
			Name,                                                                    \
			(__VEG_IMPL_CONJUNCTION(Terms)),                                         \
			__VEG_PP_TUPLE_TRANSFORM_I(__VEG_IMPL_ADD_VALUE, _, Terms))

#define VEG_DEF_CONCEPT_DISJUNCTION(Tpl, Name, Terms)                          \
	VEG_DEF_CONCEPT_BOOL_DISJUNCTION_IMPL(                                       \
			Tpl,                                                                     \
			Name,                                                                    \
			(__VEG_IMPL_DISJUNCTION(Terms)),                                         \
			__VEG_PP_TUPLE_TRANSFORM_I(__VEG_IMPL_ADD_VALUE, _, Terms))

#define __VEG_IMPL_CONJUNCTION(Tuple)                                          \
	::veg::meta::conjunction<__VEG_PP_REMOVE_PAREN(                              \
			__VEG_PP_TUPLE_TRANSFORM_I(__VEG_IMPL_PUT_TRAIT, _, Tuple))>
#define __VEG_IMPL_DISJUNCTION(Tuple)                                          \
	::veg::meta::disjunction<__VEG_PP_REMOVE_PAREN(                              \
			__VEG_PP_TUPLE_TRANSFORM_I(__VEG_IMPL_PUT_TRAIT, _, Tuple))>

#endif

////////////////////////////////////////////////////////////////////////////////

#define VEG_TEMPLATE(TParams, Constraint, Attr_Name, ...)                      \
	__VEG_IMPL_TEMPLATE(                                                         \
			Attr_Name,                                                               \
			TParams,                                                                 \
			__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Constraint),                           \
			__VA_ARGS__)

#if VEG_HAS_CONCEPTS
#define VEG_CONSTRAINED_MEMBER_FN(Constraint, Attr_Name, Params, ...)          \
	Attr_Name __VEG_PP_TUPLE_TRANSFORM_I(__VEG_IMPL_PARAM_EXPAND, _, Params)     \
	__VA_ARGS__ requires __VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Constraint)

#define VEG_CONSTRAINED_MEMBER_FN_NO_PARAM(Constraint, Attr_Name, Ret, ...)    \
	Attr_Name() __VA_ARGS__->__VEG_PP_REMOVE_PAREN(Ret) requires __VEG_PP_CAT2(  \
			__VEG_IMPL_PREFIX_, Constraint)

#define VEG_TEMPLATE_CVT(TParams, Constraint, Attr, ...)                       \
	template <__VEG_PP_REMOVE_PAREN(TParams)>                                    \
	Constraint Attr operator __VA_ARGS__()
#else
#define VEG_CONSTRAINED_MEMBER_FN(Constraint, Attr_Name, Params, ...)          \
	VEG_TEMPLATE(                                                                \
			(int _ = 0),                                                             \
			requires(                                                                \
					__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Constraint) &&                     \
					::veg::meta::bool_constant<(_ == 0)>::value),                        \
			Attr_Name,                                                               \
			__VEG_PP_REMOVE_PAREN(Params))                                           \
	__VA_ARGS__

#define VEG_CONSTRAINED_MEMBER_FN_NO_PARAM(Constraint, Attr_Name, Ret, ...)    \
	template <int _ = 0>                                                         \
	Attr_Name() __VA_ARGS__->::veg::_detail::_meta::discard_1st<                 \
			::veg::meta::enable_if_t<(                                               \
					__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Constraint) &&                     \
					::veg::meta::bool_constant<(_ == 0)>::value)>,                       \
			__VEG_PP_REMOVE_PAREN(Ret)>

#define VEG_TEMPLATE_CVT(TParams, Constraint, Attr, ...)                       \
	template <__VEG_PP_REMOVE_PAREN(TParams)>                                    \
	Attr operator ::veg::_detail::_meta::discard_1st<                            \
			::veg::meta::enable_if_t<(                                               \
					__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Constraint))>,                     \
			__VA_ARGS__>()
#endif

#if VEG_HAS_CONCEPTS && defined(__cpp_conditional_explicit) &&                 \
		(__cpp_conditional_explicit >= 201806L)
#define VEG_TEMPLATE_EXPLICIT(                                                 \
		TParams, Constraint, Explicit_Cond, Attr_Name, Params, ...)                \
	VEG_TEMPLATE(                                                                \
			TParams,                                                                 \
			Constraint,                                                              \
			Explicit_Cond Attr_Name,                                                 \
			__VEG_PP_REMOVE_PAREN(Params))                                           \
	__VA_ARGS__

#define VEG_TEMPLATE_CVT_EXPLICIT(                                             \
		TParams, Constraint, Explicit_Cond, Attr, Type, ...)                       \
	template <__VEG_PP_REMOVE_PAREN(TParams)>                                    \
	Constraint Explicit_Cond Attr operator __VEG_PP_REMOVE_PAREN(Type)()         \
			__VA_ARGS__

#else
#define VEG_TEMPLATE_EXPLICIT(                                                 \
		TParams, Constraint, Explicit_Cond, Attr_Name, Params, ...)                \
	VEG_TEMPLATE(                                                                \
			(__VEG_PP_REMOVE_PAREN TParams,                                          \
	     ::veg::meta::enable_if_t<                                               \
					 (__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Explicit_Cond)),                 \
					 int> = 0),                                                          \
			Constraint,                                                              \
			explicit Attr_Name,                                                      \
			__VEG_PP_REMOVE_PAREN(Params))                                           \
	__VA_ARGS__                                                                  \
	VEG_TEMPLATE(                                                                \
			(__VEG_PP_REMOVE_PAREN TParams,                                          \
	     ::veg::meta::enable_if_t<                                               \
					 !(__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Explicit_Cond)),                \
					 unsigned> = 0),                                                     \
			Constraint,                                                              \
			Attr_Name,                                                               \
			__VEG_PP_REMOVE_PAREN(Params))                                           \
	__VA_ARGS__

#define VEG_TEMPLATE_CVT_EXPLICIT(                                             \
		TParams, Constraint, Explicit_Cond, Attr, Type, ...)                       \
	VEG_TEMPLATE_CVT(                                                            \
			(__VEG_PP_REMOVE_PAREN TParams,                                          \
	     ::veg::meta::enable_if_t<                                               \
					 (__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Explicit_Cond)),                 \
					 int> = 0),                                                          \
			Constraint,                                                              \
			explicit Attr,                                                           \
			__VEG_PP_REMOVE_PAREN(Type))                                             \
	__VA_ARGS__                                                                  \
                                                                               \
	VEG_TEMPLATE_CVT(                                                            \
			(__VEG_PP_REMOVE_PAREN TParams,                                          \
	     ::veg::meta::enable_if_t<                                               \
					 !(__VEG_PP_CAT2(__VEG_IMPL_PREFIX_, Explicit_Cond)),                \
					 unsigned> = 0),                                                     \
			Constraint,                                                              \
			Attr,                                                                    \
			__VEG_PP_REMOVE_PAREN(Type))                                             \
	__VA_ARGS__
#endif

#define __VEG_IMPL_PREFIX_requires
#define __VEG_IMPL_PREFIX_explicit

#define __VEG_IMPL_PARAM_EXPAND(I, _, Param)                                   \
	__VEG_PP_TAIL Param __VEG_PP_HEAD Param
#if VEG_HAS_CONCEPTS
#define __VEG_IMPL_TEMPLATE(Attr_Name, TParams, Constraint, ...)               \
	template <__VEG_PP_REMOVE_PAREN(TParams)>                                    \
	requires Constraint Attr_Name __VEG_PP_TUPLE_TRANSFORM_I(                    \
			__VEG_IMPL_PARAM_EXPAND, _, (__VA_ARGS__))
#else

#define __VEG_IMPL_TEMPLATE2_HELPER_0(Constraint, Param)                       \
	__VEG_PP_TAIL Param __VEG_PP_HEAD Param

#define __VEG_IMPL_TEMPLATE2_HELPER_1(Constraint, Param)                       \
	::veg::_detail::_meta::                                                      \
			discard_1st<::veg::meta::enable_if_t<(Constraint)>, __VEG_PP_TAIL Param> \
					__VEG_PP_HEAD Param

#define __VEG_IMPL_TEMPLATE2_HELPER(I, Constraint, Param)                      \
	__VEG_PP_CAT2(                                                               \
			__VEG_IMPL_TEMPLATE2_HELPER_, __VEG_IMPL_PP_IS_1(__VEG_IMPL_PP_INC(I)))  \
	(Constraint, Param)

#define __VEG_IMPL_TEMPLATE(Attr_Name, TParams, Constraint, ...)               \
	template <__VEG_PP_REMOVE_PAREN(TParams)>                                    \
	Attr_Name __VEG_PP_TUPLE_TRANSFORM_I(                                        \
			__VEG_IMPL_TEMPLATE2_HELPER, Constraint, (__VA_ARGS__))

#endif

////////////////////////////////////////////////////////////////////////////////

#if __cplusplus >= 201703L
#define VEG_INLINE_VAR(Name, Obj)                                              \
	inline constexpr auto const& Name = ::veg::meta::static_const<Obj>::value;   \
	static_assert((void(Name), true), ".")

#define VEG_INLINE_VAR_TEMPLATE(Tpl, Name, ...) /* NOLINT */                   \
	template <__VEG_PP_REMOVE_PAREN(Tpl)>                                        \
	inline constexpr auto const& Name =                                          \
			::veg::meta::static_const<__VA_ARGS__>::value;                           \
	VEG_NOM_SEMICOLON /* NOLINT */
#else
#define VEG_INLINE_VAR(Name, Obj)                                              \
	namespace /* NOLINT */ {                                                     \
	constexpr auto const& Name = ::veg::meta::static_const<Obj>::value;          \
	}                                                                            \
	static_assert((void(Name), true), ".")

#if __cplusplus >= 201402L
#define VEG_INLINE_VAR_TEMPLATE(Tpl, Name, ...) /* NOLINT */                   \
	namespace /* NOLINT */ {                      /* NOLINT */                   \
	template <__VEG_PP_REMOVE_PAREN(Tpl)>                                        \
	constexpr auto const& Name =                       /* NOLINT */              \
			::veg::meta::static_const<__VA_ARGS__>::value; /* NOLINT */              \
	}                                                                            \
	VEG_NOM_SEMICOLON /* NOLINT */
#else
#define VEG_INLINE_VAR_TEMPLATE(Tpl, Name, ...) VEG_NOM_SEMICOLON
#endif
#endif

#define VEG_NIEBLOID(Name) VEG_INLINE_VAR(Name, nb::Name) // NOLINT

#define VEG_NIEBLOID_TEMPLATE(Tpl, Name, ...)                                  \
	VEG_INLINE_VAR_TEMPLATE(Tpl, Name, nb::Name<__VA_ARGS__>) // NOLINT

#define VEG_TAG(Name, Type)                                                    \
	namespace _ {                                                                \
	template <int I>                                                             \
	struct Type {                                                                \
		explicit Type() = default;                                                 \
	};                                                                           \
	}                                                                            \
	using Type = _::Type<0>;                                                     \
	VEG_INLINE_VAR(Name, Type)

#define VEG_TAG_TEMPLATE(Tpl, Name, Type, ...)                                 \
	template <__VEG_PP_REMOVE_PAREN(Tpl)>                                        \
	struct Type {                                                                \
		explicit Type() = default;                                                 \
	};                                                                           \
	VEG_INLINE_VAR_TEMPLATE(Tpl, Name, Type<__VA_ARGS__>)

#define VEG_FWD(X) static_cast<decltype(X)&&>(X)
#define VEG_FWD2(X) static_cast<decltype(X)>(static_cast<decltype(X)&&>(X))

// disallows moving const rvalues
#define VEG_MOV(X) static_cast<typename ::veg::uncvref_t<decltype(X)>&&>(X)

#ifdef VEG_NO_INSTANTIATE
#define VEG_INSTANTIATE(Fn, ...) VEG_NOM_SEMICOLON
#define VEG_INSTANTIATE_CLASS(Class, ...) VEG_NOM_SEMICOLON
#else
#define VEG_INSTANTIATE(Fn, ...)                                               \
	__VEG_IMPL_INSTANTIATE(                                                      \
			Fn,                                                                      \
			__VEG_PP_CAT(                                                            \
					__VEG_PP_CAT(_dummy_explicit_instantiation, __LINE__),               \
					__VEG_PP_CAT(_, __COUNTER__)),                                       \
			__VA_ARGS__)
#define __VEG_IMPL_INSTANTIATE(Fn, Name, ...)                                  \
	template <typename... Ts>                                                    \
	struct Name {                                                                \
		void apply(Ts&&... args) { Fn(VEG_FWD(args)...); }                         \
	};                                                                           \
	template struct Name<__VA_ARGS__>

#define VEG_INSTANTIATE_CLASS(Class, ...) template struct Class<__VA_ARGS__>
#endif

#define VEG_NOM_SEMICOLON static_assert(true, ".")

namespace veg {
namespace meta {
template <typename...>
using void_t = void;
} // namespace meta
namespace _detail {
namespace _meta {

template <bool B, typename T = void>
struct enable_if {
	using type = T;
};
template <typename T>
struct enable_if<false, T> {};

template <typename U, typename V>
using discard_1st = V;

template <typename T>
struct uncvlref;
template <typename T>
struct uncvlref<T&> {
	using type = T;
};
template <typename T>
struct uncvlref<T const&> {
	using type = T;
};
template <typename T>
struct uncvlref<T volatile&> {
	using type = T;
};
template <typename T>
struct uncvlref<T volatile const&> {
	using type = T;
};

template <typename T>
struct unref;
template <typename T>
struct unref<T&> {
	using type = T;
};

template <typename T>
auto declval() VEG_ALWAYS_NOEXCEPT -> T;
} // namespace _meta
} // namespace _detail

namespace meta {
template <typename T>
struct static_const {
	static constexpr T value{};
};

template <typename T>
constexpr T static_const<T>::value; // NOLINT(readability-redundant-declaration)
} // namespace meta

template <typename... Ts>
struct incomplete_t;

template <typename... Types, typename... Args>
auto print_types_halt(Args&&...) -> incomplete_t<Types..., Args...>;
template <typename... Types, typename... Args>
VEG_DEPRECATED("")
VEG_CPP14(constexpr) void print_types(Args&&... /*unused*/) {}

namespace nb {
struct unused {
	template <typename... Ts>
	VEG_CPP14(constexpr)
	void operator()(Ts const&... /*unused*/) const VEG_NOEXCEPT {}
};
} // namespace nb
VEG_NIEBLOID(unused);

using usize = decltype(sizeof(0));
namespace _detail {

template <isize I>
struct EmptyI {};

using Empty = EmptyI<0>;
using EmptyArr = Empty[];
namespace _meta {

template <typename T, T... Nums>
struct integer_sequence;

#if VEG_HAS_BUILTIN(__make_integer_seq)

template <typename T, T N>
using make_integer_sequence = __make_integer_seq<integer_sequence, T, N>;

#elif __GNUC__ >= 8

template <typename T, T N>
using make_integer_sequence = integer_sequence<T, __integer_pack(N)...>;

#else

namespace _detail {

template <typename Seq1, typename Seq2>
struct _merge;

template <typename Seq1, typename Seq2>
struct _merge_p1;

template <typename T, T... Nums1, T... Nums2>
struct _merge<integer_sequence<T, Nums1...>, integer_sequence<T, Nums2...>> {
	using type = integer_sequence<T, Nums1..., (sizeof...(Nums1) + Nums2)...>;
};

template <typename T, T... Nums1, T... Nums2>
struct _merge_p1<integer_sequence<T, Nums1...>, integer_sequence<T, Nums2...>> {
	using type = integer_sequence<
			T,
			Nums1...,
			(sizeof...(Nums1) + Nums2)...,
			sizeof...(Nums1) + sizeof...(Nums2)>;
};

template <typename T, usize N, bool Even = (N % 2) == 0>
struct _make_integer_sequence {
	using type = typename _merge<
			typename _make_integer_sequence<T, N / 2>::type,
			typename _make_integer_sequence<T, N / 2>::type>::type;
};

template <typename T, usize N>
struct _make_integer_sequence<T, N, false> {
	using type = typename _merge_p1<
			typename _make_integer_sequence<T, N / 2>::type,
			typename _make_integer_sequence<T, N / 2>::type>::type;
};

template <typename T>
struct _make_integer_sequence<T, 0> {
	using type = integer_sequence<T>;
};
template <typename T>
struct _make_integer_sequence<T, 1> {
	using type = integer_sequence<T, 0>;
};

} // namespace _detail

template <typename T, T N>
using make_integer_sequence =
		typename _detail::_make_integer_sequence<T, N>::type;

#endif

#define VEG_DEF_CONCEPT_BUILTIN_OR_INTERNAL(Tpl, Name, ...)                    \
	VEG_DEF_CONCEPT(                                                             \
			Tpl,                                                                     \
			Name,                                                                    \
			VEG_HAS_BUILTIN_OR(                                                      \
					__is_##Name,                                                         \
					__is_##Name(__VA_ARGS__),                                            \
					(_detail::_meta::is_##Name<__VA_ARGS__>::value)))

template <usize N>
using make_index_sequence = make_integer_sequence<usize, N>;

template <typename... Ts>
struct type_sequence;

} // namespace _meta

template <usize I, typename T>
struct SimpleLeaf {
	T inner;
};

template <typename Seq, typename... Ts>
struct SimpleITuple;

template <usize... Is, typename... Ts>
struct SimpleITuple<_meta::integer_sequence<usize, Is...>, Ts...>
		: SimpleLeaf<Is, Ts>... {
#if __cplusplus < 201703
	constexpr SimpleITuple(Ts... args) noexcept
			: SimpleLeaf<Is, Ts>{Ts(VEG_FWD(args))}... {}
#endif
};

template <typename... Ts>
using SimpleTuple =
		SimpleITuple<_meta::make_index_sequence<sizeof...(Ts)>, Ts...>;

template <typename... Ts>
constexpr auto make_simple_tuple(Empty /*dummy*/, Ts... args) noexcept
		-> SimpleTuple<Ts...> {
#if __cplusplus < 201703
	return {Ts(VEG_FWD(args))...};
#else
	return {{Ts(VEG_FWD(args))}...};
#endif
}

template <typename T>
struct mem_ptr_type;
template <typename C, typename Mem>
struct mem_ptr_type<Mem C::*> {
	using Type = Mem;
};

constexpr auto all_of_slice(bool const* arr, usize size) VEG_NOEXCEPT -> bool {
	return size == 0 ? true
	                 : (arr[0] && _detail::all_of_slice(arr + 1, size - 1));
}
template <usize N>
inline constexpr auto all_of(bool const (&lst)[N]) VEG_NOEXCEPT -> bool {
	return _detail::all_of_slice(lst, N);
}

template <typename T>
struct member_extract_access {
	template <typename U, typename = U>
	struct DetectImpl {
		static constexpr bool value = false;
		using Type = void;
	};
	template <typename U>
	struct DetectImpl<U, typename U::_veglib_impl_member_extract::Type> {
		static constexpr bool value = true;
		using Type = typename U::_veglib_impl_member_extract;
	};

	using Detect = DetectImpl<T>;
	static constexpr bool value = Detect::value;
	using Type = typename Detect::Type;
};
} // namespace _detail
namespace meta {
template <bool B, typename T = void>
using enable_if_t = typename _detail::_meta::enable_if<B, T>::type;

template <typename T>
using uncvref_t = typename _detail::_meta::uncvlref<T&>::type;
} // namespace meta
using meta::uncvref_t;

namespace meta {
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
} // namespace meta
namespace _detail {
namespace _meta {

struct wrapper_base {
	static auto test(...) -> meta::false_type;
};
template <typename T>
struct wrapper : wrapper_base {
	using wrapper_base::test;
	static auto test(wrapper<T>*) -> meta::true_type;
};
template <typename T, typename U>
using is_same = decltype(wrapper<T>::test(static_cast<wrapper<U>*>(nullptr)));
} // namespace _meta
} // namespace _detail
namespace concepts {
VEG_DEF_CONCEPT_BUILTIN_OR_INTERNAL((typename T, typename U), same, T, U);
} // namespace concepts
} // namespace veg

#define VEG_DECLTYPE_VOID(...) decltype(void(__VA_ARGS__))
#define VEG_BOOL_NOEXCEPT(...) ::veg::meta::bool_constant<noexcept(__VA_ARGS__)>

#define VEG_CHECK_CONCEPT(...)                                                 \
	VEG_CHECK_CONCEPT_MACRO(::veg::concepts, __VA_ARGS__)
#define VEG_CONCEPT(...) VEG_CONCEPT_MACRO(::veg::concepts, __VA_ARGS__)

#define __VEG_IMPL_GET_MEMBER_PTR(_, MemberPtr) /* NOLINT */ , &Type::MemberPtr
#define __VEG_IMPL_GET_MEMBER_NAME_PTR(_, MemberPtr) /* NOLINT */              \
	static_cast<char const*>(__VEG_PP_STRINGIZE(MemberPtr)),
#define __VEG_IMPL_GET_MEMBER_NAME_LEN(_, MemberPtr) /* NOLINT */              \
	(sizeof(__VEG_PP_STRINGIZE(MemberPtr)) - 1),

#define __VEG_IMPL_STRUCT_SETUP(PClass, ...) /* NOLINT */                      \
	void _veg_lib_name_test()&& noexcept {                                       \
		static_assert(                                                             \
				VEG_CONCEPT(same<decltype(this), __VEG_PP_REMOVE_PAREN(PClass)*>),     \
				"struct mismatch in VEG_REFLECT");                                     \
	}                                                                            \
	struct _veglib_impl_member_extract {                                         \
		using Type = __VEG_PP_REMOVE_PAREN(PClass);                                \
		using MemberTuple = decltype(::veg::_detail::make_simple_tuple(            \
				::veg::_detail::Empty {} __VEG_PP_TUPLE_FOR_EACH(                      \
						__VEG_IMPL_GET_MEMBER_PTR, _, (__VA_ARGS__))));                    \
		static constexpr auto member_pointers() noexcept -> MemberTuple {          \
			return ::veg::_detail::make_simple_tuple(::veg::_detail::Empty {         \
			} __VEG_PP_TUPLE_FOR_EACH(__VEG_IMPL_GET_MEMBER_PTR, _, (__VA_ARGS__))); \
		}                                                                          \
		static constexpr char const* class_name_ptr =                              \
				__VEG_PP_STRINGIZE (__VEG_PP_REMOVE_PAREN(PClass));                     \
		static constexpr ::veg::usize class_name_len =                             \
				sizeof(__VEG_PP_STRINGIZE (__VEG_PP_REMOVE_PAREN(PClass))) - 1;         \
		static constexpr char const* member_name_ptrs[] = {                        \
				__VEG_PP_TUPLE_FOR_EACH(                                               \
						__VEG_IMPL_GET_MEMBER_NAME_PTR, _, (__VA_ARGS__))};                \
		static constexpr ::veg::usize member_name_lens[] = {                       \
				__VEG_PP_TUPLE_FOR_EACH(                                               \
						__VEG_IMPL_GET_MEMBER_NAME_LEN, _, (__VA_ARGS__))};                \
	};                                                                           \
	friend struct ::veg::_detail::member_extract_access<__VEG_PP_REMOVE_PAREN(   \
			PClass)>;                                                                \
	VEG_NOM_SEMICOLON

#define VEG_REFLECT(PClass, ...) __VEG_IMPL_STRUCT_SETUP(PClass, __VA_ARGS__)

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_MACROS_HPP_HSTLSKZXS */
