#ifndef VEG_DELETE_SPECIAL_MEMBERS_HPP_2HKRCVWUS
#define VEG_DELETE_SPECIAL_MEMBERS_HPP_2HKRCVWUS

#include "veg/internal/prologue.hpp"
#include "veg/internal/typedefs.hpp"

namespace veg {
namespace _detail {
struct NoDefaultCtor {
	NoDefaultCtor() = delete;
};
struct NoMoveAssign {
	NoMoveAssign() = default;
	~NoMoveAssign() = default;
	NoMoveAssign(NoMoveAssign const&) = default;
	NoMoveAssign(NoMoveAssign&&) = default;
	auto operator=(NoMoveAssign const&) -> NoMoveAssign& = default;
	auto operator=(NoMoveAssign&&) -> NoMoveAssign& = delete;
};
struct NoCopyAssign {
	NoCopyAssign() = default;
	~NoCopyAssign() = default;
	NoCopyAssign(NoCopyAssign const&) = default;
	NoCopyAssign(NoCopyAssign&&) = default;
	auto operator=(NoCopyAssign const&) -> NoCopyAssign& = delete;
	auto operator=(NoCopyAssign&&) -> NoCopyAssign& = default;
};
struct NoMoveCtor {
	NoMoveCtor() = default;
	~NoMoveCtor() = default;
	NoMoveCtor(NoMoveCtor const&) = default;
	NoMoveCtor(NoMoveCtor&&) = delete;
	auto operator=(NoMoveCtor const&) -> NoMoveCtor& = default;
	auto operator=(NoMoveCtor&&) -> NoMoveCtor& = default;
};
struct NoCopyCtor {
	NoCopyCtor() = default;
	~NoCopyCtor() = default;
	NoCopyCtor(NoCopyCtor const&) = delete;
	NoCopyCtor(NoCopyCtor&&) = default;
	auto operator=(NoCopyCtor const&) -> NoCopyCtor& = default;
	auto operator=(NoCopyCtor&&) -> NoCopyCtor& = default;
};

} // namespace _detail
} // namespace veg

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_DELETE_SPECIAL_MEMBERS_HPP_2HKRCVWUS */
