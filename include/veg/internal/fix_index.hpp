#ifndef VEG_META_INT_FIX_HPP_7S9Y48TFS
#define VEG_META_INT_FIX_HPP_7S9Y48TFS

#include "veg/type_traits/tags.hpp"
#include "veg/internal/std.hpp"
#include "veg/internal/fmt.hpp"
#include "veg/util/compare.hpp"
#include "veg/internal/prologue.hpp"

namespace veg {
struct Dyn;
template <isize N>
struct Fix;

namespace _detail {
template <typename L, typename R>
struct binary_traits {
	using Add = void;
	using Sub = void;
	using Mul = void;
	using Div = void;
	using Mod = void;

	using CmpEq = void;
	using CmpNEq = void;
	using CmpLT = void;
	using CmpLE = void;
	using CmpGT = void;
	using CmpGE = void;
};

namespace idx {
namespace adl {
template <typename T>
struct IdxBase {};
} // namespace adl
} // namespace idx
namespace _meta {
template <typename T>
struct is_fix : false_type {};
template <isize N>
struct is_fix<Fix<N>> : true_type {};
} // namespace _meta
} // namespace _detail

namespace concepts {
VEG_DEF_CONCEPT(
		typename T,
		index,
		VEG_CONCEPT(same<T, Dyn>) || _detail::_meta::is_fix<T>::value);
} // namespace concepts

enum struct Ternary : unsigned char {
	no,
	maybe,
	yes,
};

constexpr auto no = Ternary::no;
constexpr auto maybe = Ternary::maybe;
constexpr auto yes = Ternary::yes;
using no_c = meta::constant<Ternary, Ternary::no>;
using maybe_c = meta::constant<Ternary, Ternary::maybe>;
using yes_c = meta::constant<Ternary, Ternary::yes>;

template <Ternary T>
struct Boolean;

template <Ternary T>
struct Boolean {
	constexpr Boolean() VEG_NOEXCEPT = default;
	using type = meta::constant<Ternary, T>;

	VEG_INLINE constexpr Boolean(Boolean<maybe> /*b*/, Unsafe /*tag*/)
			VEG_NOEXCEPT;
	VEG_INLINE constexpr Boolean // NOLINT(hicpp-explicit-conversions)
			(Boolean<maybe> b) VEG_NOEXCEPT;

	VEG_NODISCARD VEG_INLINE constexpr friend auto
	operator!(Boolean /*arg*/) VEG_NOEXCEPT -> Boolean<T == yes ? no : yes> {
		return {};
	}
	VEG_NODISCARD VEG_INLINE explicit constexpr
	operator bool() const VEG_NOEXCEPT {
		return T == yes;
	}

private:
	void print(fmt::Buffer& out) const {
		constexpr auto const& yes_str = "maybe(true)";
		constexpr auto const& no_str = "maybe(false)";
		char const* str = (T == yes) ? yes_str : no_str;
		auto len = ((T == yes) ? sizeof(yes_str) : sizeof(no_str)) - 1;
		out.insert(out.size(), str, (len));
	}
};

template <isize N>
struct Fix : _detail::idx::adl::IdxBase<Fix<N>> {
	constexpr Fix() VEG_NOEXCEPT = default;
	VEG_INLINE constexpr Fix(Dyn /*arg*/, Unsafe /*tag*/) VEG_NOEXCEPT;
	VEG_INLINE constexpr Fix // NOLINT(hicpp-explicit-conversions)
			(Dyn arg) VEG_NOEXCEPT;
	VEG_TEMPLATE(
			(isize M), requires((M != N)), constexpr Fix, (/*arg*/, Fix<M>)) = delete;

	VEG_NODISCARD VEG_INLINE explicit constexpr
	operator isize() const VEG_NOEXCEPT {
		return N;
	}
	VEG_NODISCARD VEG_INLINE constexpr auto operator+() const VEG_NOEXCEPT
			-> Fix {
		return {};
	}
	VEG_NODISCARD VEG_INLINE constexpr auto operator-() const VEG_NOEXCEPT
			-> Fix<-N> {
		return {};
	}

#define VEG_OP(Op, Name, TypeName)                                             \
	VEG_TEMPLATE(                                                                \
			(typename R),                                                            \
			requires(VEG_CONCEPT(index<R>)),                                         \
			VEG_NODISCARD VEG_INLINE constexpr auto                                  \
			operator Op,                                                             \
			(b, R))                                                                  \
	const VEG_NOEXCEPT->typename _detail::binary_traits<Fix, R>::TypeName {      \
		return _detail::binary_traits<Fix, R>::Name##_fn(*this, b);                \
	}                                                                            \
	VEG_NOM_SEMICOLON

	VEG_OP(+, add, Add);
	VEG_OP(-, sub, Sub);
	VEG_OP(*, mul, Mul);

#undef VEG_OP

	VEG_TEMPLATE(
			(typename R),
			requires(
					VEG_CONCEPT(index<R>) &&
					VEG_CONCEPT(index<typename _detail::binary_traits<Fix, R>::Div>)),
			VEG_NODISCARD VEG_INLINE constexpr auto
			operator/,
			(b, R))
	const VEG_NOEXCEPT->typename _detail::binary_traits<Fix, R>::Div {
		return _detail::binary_traits<Fix, R>::div_fn(*this, b);
	}

	VEG_TEMPLATE(
			(typename R),
			requires(
					VEG_CONCEPT(index<R>) &&
					VEG_CONCEPT(index<typename _detail::binary_traits<Fix, R>::Mod>)),
			VEG_NODISCARD VEG_INLINE constexpr auto
			operator%,
			(b, R))
	const VEG_NOEXCEPT->typename _detail::binary_traits<Fix, R>::Mod {
		return _detail::binary_traits<Fix, R>::mod_fn(*this, b);
	}

#define VEG_CMP(Name, TypeName, Op)                                            \
	VEG_TEMPLATE(                                                                \
			(typename R),                                                            \
			requires(VEG_CONCEPT(index<R>)),                                         \
			VEG_NODISCARD VEG_INLINE constexpr auto                                  \
			operator Op, /* NOLINT */                                                \
			(b, R))                                                                  \
	const VEG_NOEXCEPT->typename _detail::binary_traits<Fix, R>::TypeName {      \
		return _detail::binary_traits<Fix, R>::cmp_##Name##_fn(*this, b);          \
	}                                                                            \
	VEG_NOM_SEMICOLON

	VEG_CMP(eq, CmpEq, ==);
	VEG_CMP(neq, CmpNEq, !=);
	VEG_CMP(lt, CmpLT, <);
	VEG_CMP(le, CmpLE, <=);
	VEG_CMP(gt, CmpGT, >);
	VEG_CMP(ge, CmpGE, >=);

#undef VEG_CMP

#undef VEG_CMP
};

namespace _detail {
struct Error {
	constexpr auto operator()(u64 const* fail = nullptr) const VEG_NOEXCEPT
			-> u64 {
		return *fail;
	}
};

using parser = auto (*)(char, Error) -> u64;
constexpr auto parse_digit_2(char c, Error e) VEG_NOEXCEPT -> u64 {
	return (c == '0') ? 0 : (c == '1' ? 1 : e());
}
constexpr auto parse_digit_8(char c, Error e) VEG_NOEXCEPT -> u64 {
	return (c >= '0' && c <= '7') ? u64(c - '0') : e();
}
constexpr auto parse_digit_10(char c, Error e) VEG_NOEXCEPT -> u64 {
	return (c >= '0' && c <= '9') ? u64(c - '0') : e();
}
constexpr auto parse_digit_16(char c, Error e) VEG_NOEXCEPT -> u64 {
	return (c >= '0' && c <= '9') //
	           ? u64(c - '0')
	           : (c >= 'a' && c <= 'f') //
	                 ? u64(c - 'a')
	                 : (c >= 'A' && c <= 'F') //
	                       ? u64(c - 'A')
	                       : e();
}

constexpr auto parse_digit(u64 radix) VEG_NOEXCEPT -> parser {
	return radix == 2
	           ? parse_digit_2
	           : (radix == 8
	                  ? parse_digit_8
	                  : (radix == 10 ? parse_digit_10
	                                 : (radix == 16 ? parse_digit_16 : nullptr)));
}

constexpr auto
parse_num(char const* str, u64 len, u64 radix, Error e) VEG_NOEXCEPT -> u64 {
	return (len == 0) ? 0
	                  : radix * parse_num(str, len - 1, radix, e) +
	                        (parse_digit(radix)(str[len - 1], e));
}

constexpr auto parse_int(char const* str, u64 len, Error e) VEG_NOEXCEPT
		-> u64 {
	return (len == 0) //
	           ? e()
	           : ((str[0] == '0')   //
	                  ? ((len == 1) //
	                         ? 0
	                         : (str[1] == 'b' || str[1] == 'B') //
	                               ? parse_num(str + 2, len - 2, 2, e)
	                               : (str[1] == 'x' || str[1] == 'X') //
	                                     ? parse_num(str + 2, len - 2, 16, e)
	                                     : parse_num(str + 1, len - 1, 8, e))
	                  : parse_num(str, len, 10, e));
}

template <char... Chars>
struct char_seq {
	static constexpr char value[] = {Chars...};
};

template <isize N, isize M>
struct binary_traits<Fix<N>, Fix<M>> {

#define VEG_OP(Name, TypeName, Op)                                             \
	using TypeName /* NOLINT(bugprone-macro-parentheses) */ =                    \
			Fix<isize(usize(isize{N}) Op usize(isize{M}))>;                          \
	VEG_NODISCARD VEG_INLINE static constexpr auto Name##_fn(Fix<N>, Fix<M>)     \
			VEG_NOEXCEPT->TypeName {                                                 \
		return {};                                                                 \
	}                                                                            \
	static_assert(true, "")

#define VEG_CMP(Name, TypeName, Op)                                            \
	using TypeName /* NOLINT(bugprone-macro-parentheses) */ =                    \
			Boolean<(N Op M) ? yes : no>;                                            \
	VEG_NODISCARD VEG_INLINE static constexpr auto Name##_fn(Fix<N>, Fix<M>)     \
			VEG_NOEXCEPT->TypeName {                                                 \
		return {};                                                                 \
	}                                                                            \
	static_assert(true, "")

	VEG_OP(add, Add, +);
	VEG_OP(sub, Sub, -);
	VEG_OP(mul, Mul, *);
	VEG_CMP(cmp_eq, CmpEq, ==);
	VEG_CMP(cmp_neq, CmpNEq, !=);
	VEG_CMP(cmp_lt, CmpLT, <);
	VEG_CMP(cmp_le, CmpLE, <=);
	VEG_CMP(cmp_gt, CmpGT, >);
	VEG_CMP(cmp_ge, CmpGE, >=);

	using Div = meta::conditional_t<M == 0, void, Fix<N / (M != 0 ? M : 1)>>;
	using Mod = meta::conditional_t<M == 0, void, Fix<N % (M != 0 ? M : 1)>>;

	VEG_NODISCARD VEG_INLINE static constexpr auto
	div_fn(Fix<N> /*a*/, Fix<M> /*b*/) VEG_NOEXCEPT -> Div {
		return Div();
	}
	VEG_NODISCARD VEG_INLINE static constexpr auto
	mod_fn(Fix<N> /*a*/, Fix<M> /*b*/) VEG_NOEXCEPT -> Mod {
		return Mod();
	}

#undef VEG_OP
#undef VEG_CMP
};
namespace idx {
namespace adl {} // namespace adl
} // namespace idx
} // namespace _detail

inline namespace literals {
template <char... Chars>
VEG_INLINE constexpr auto
operator"" _c() VEG_NOEXCEPT -> Fix<_detail::parse_int(
		_detail::char_seq<Chars...>::value, sizeof...(Chars), _detail::Error{})> {
	return {};
}
} // namespace literals

template <>
struct fmt::Debug<Boolean<yes>> {
	static void to_string(fmt::Buffer& out, Ref<Boolean<yes>> /*val*/) {
		out.insert(out.size(), "yes", 3);
	}
};
template <>
struct fmt::Debug<Boolean<no>> {
	static void to_string(fmt::Buffer& out, Ref<Boolean<no>> /*val*/) {
		out.insert(out.size(), "no", 2);
	}
};

template <isize N>
struct fmt::Debug<Fix<N>> {
	static void to_string(fmt::Buffer& out, Ref<Fix<N>> /*val*/) {
		out.insert(out.size(), "Fix[", 4);
		Debug<isize>::to_string(out, ref(N));
		out.insert(out.size(), "]", 1);
	}
};
} // namespace veg

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_META_INT_FIX_HPP_7S9Y48TFS */
