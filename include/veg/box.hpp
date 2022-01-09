#ifndef VEG_BOX_HPP_HUJDLY6PS
#define VEG_BOX_HPP_HUJDLY6PS

#include "veg/internal/delete_special_members.hpp"
#include "veg/type_traits/constructible.hpp"
#include "veg/memory/alloc.hpp"
#include "veg/tuple.hpp"
#include "veg/internal/prologue.hpp"

namespace veg {
namespace internal {
namespace _mem {
template <typename T>
struct UniquePtr {
	T* inner = nullptr;
	UniquePtr() = default;
	UniquePtr(UniquePtr&& rhs) noexcept : inner{rhs.inner} { rhs.inner = {}; }
	VEG_INLINE UniquePtr(UniquePtr const& /*unused*/) noexcept {}
	auto operator=(UniquePtr const&) -> UniquePtr& = delete;
	auto operator=(UniquePtr&&) -> UniquePtr& = delete;
	~UniquePtr() = default;
};

template <typename T, typename A>
struct BoxAlloc : Tuple<A, UniquePtr<T>> {
	BoxAlloc() = default;

	using Tuple<A, UniquePtr<T>>::Tuple;

	BoxAlloc(BoxAlloc&&) = default;
	BoxAlloc(BoxAlloc const&) = default;
	auto operator=(BoxAlloc&&) -> BoxAlloc& = delete;
	auto operator=(BoxAlloc const&) -> BoxAlloc& = delete;

	VEG_INLINE ~BoxAlloc() {
		if ((*this)[1_c].inner != nullptr) {
			mem::Alloc<A>::dealloc(
					mut((*this)[0_c]),
					static_cast<void*>((*this)[1_c].inner),
					mem::Layout{sizeof(T), alignof(T)});
		}
	}
};

template <typename T>
struct DerefCopyFn {
	T const* ptr;
	VEG_INLINE auto
	operator()() const&& VEG_NOEXCEPT_IF(VEG_CONCEPT(nothrow_copyable<T>)) -> T {
		return T(*ptr);
	}
};
} // namespace _mem
} // namespace internal

namespace mem {
template <
		typename T,
		typename A = SystemAlloc,
		bool NoThrowCopy = VEG_CONCEPT(nothrow_copyable<T>) &&
                       VEG_CONCEPT(nothrow_copy_assignable<T>)>
struct BoxIncomplete {
	internal::_mem::BoxAlloc<T, A> _;

	VEG_INLINE constexpr auto alloc_ref() const noexcept -> Ref<A> {
		return ref(_[0_c]);
	}
	VEG_INLINE VEG_CPP14(constexpr) auto alloc_mut(Unsafe /*tag*/) noexcept
			-> RefMut<A> {
		return mut(_[0_c]);
	}
	VEG_INLINE constexpr auto ptr_ref() const noexcept -> Ref<T*> {
		return ref(_[1_c].inner);
	}
	VEG_INLINE VEG_CPP14(constexpr) auto ptr_ref_mut(Unsafe /*tag*/) noexcept
			-> RefMut<T*> {
		return mut(_[1_c].inner);
	}

	VEG_INLINE constexpr auto ptr() const noexcept -> T const* {
		return ptr_ref().get();
	}
	VEG_INLINE VEG_CPP14(constexpr) auto ptr_mut() noexcept -> T* {
		return ptr_ref().get();
	}

	VEG_INLINE constexpr auto operator*() const noexcept -> T const& {
		return *ptr_ref().get();
	}
	VEG_INLINE VEG_CPP14(constexpr) auto operator*() noexcept -> T& {
		return *ptr_ref().get();
	}
	VEG_INLINE constexpr auto operator->() const noexcept -> T const* {
		return ptr_ref().get();
	}
	VEG_INLINE VEG_CPP14(constexpr) auto operator->() noexcept -> T* {
		return ptr_ref().get();
	}

	VEG_TEMPLATE(
			(int _, typename FnT, typename FnA),
			requires(VEG_CONCEPT(fn_once<FnT, T>) && VEG_CONCEPT(fn_once<FnA, A>)),
			BoxIncomplete,
			(/*tag*/, _::FromRawParts<_>),
			(/*tag*/, Unsafe),
			(fn_a, FnA),
			(has_value, bool),
			(fn_t, FnT))
	VEG_NOEXCEPT_IF(
			(VEG_CONCEPT(nothrow_fn_once<FnT, T>) &&
	     VEG_CONCEPT(nothrow_fn_once<FnA, A>)))

			: _{
						InPlace<Tuplify>{},
						VEG_FWD(fn_a),
						internal::DefaultFn<internal::_mem::UniquePtr<T>>{},
				} {
		if (!has_value) {
			return;
		}
		mem::Layout l{
				sizeof(T),
				alignof(T),
		};

		internal::_mem::ManagedAlloc<A> block{
				mem::Alloc<A>::alloc(this->alloc_mut(unsafe), l).data,
				l,
				this->alloc_mut(unsafe),
		};

		mem::construct_with(static_cast<T*>(block.data), VEG_FWD(fn_t));

		this->ptr_ref_mut(unsafe).get() = static_cast<T*>(block.data);
		block.data = nullptr;
	}

	BoxIncomplete() = default;
	BoxIncomplete(BoxIncomplete&&) = default;

	VEG_INLINE BoxIncomplete(BoxIncomplete const& rhs) VEG_NOEXCEPT_IF(
			VEG_CONCEPT(nothrow_copyable<A>) &&
			VEG_CONCEPT(alloc::nothrow_alloc<A>) && NoThrowCopy)
			: BoxIncomplete{
						from_raw_parts,
						unsafe,
						internal::CopyFn<A>{rhs.alloc_ref().get()},
						(rhs.ptr_ref().get() != nullptr),
						internal::_mem::DerefCopyFn<T>{rhs.ptr_ref().get()},
				} {
		static_assert(
				NoThrowCopy == (VEG_CONCEPT(nothrow_copyable<T>) &&
		                    VEG_CONCEPT(nothrow_copy_assignable<T>)),
				".");
	}

	auto operator=(BoxIncomplete&& rhs) noexcept -> BoxIncomplete& {
		BoxIncomplete tmp = VEG_FWD(rhs);
		{ auto cleanup = static_cast<BoxIncomplete&&>(*this); }
		this->alloc_mut(unsafe).get() =
				static_cast<A&&>(tmp.alloc_mut(unsafe).get());
		this->ptr_ref_mut(unsafe).get() = tmp.ptr_ref().get();

		tmp.ptr_ref_mut(unsafe).get() = nullptr;
		return *this;
	}

	auto operator=(BoxIncomplete const& rhs) VEG_NOEXCEPT_IF(
			VEG_CONCEPT(alloc::nothrow_alloc<A>) &&
			VEG_CONCEPT(nothrow_copy_assignable<A>) && NoThrowCopy)
			-> BoxIncomplete& {
		static_assert(
				NoThrowCopy == (VEG_CONCEPT(nothrow_copyable<T>) &&
		                    VEG_CONCEPT(nothrow_copy_assignable<T>)),
				".");
		if (this != mem::addressof(rhs)) {
			if (cmp::eq(this->alloc_ref(), rhs.alloc_ref()) &&
			    this->ptr_ref().get() != nullptr && rhs.ptr_ref().get() != nullptr) {
				*(this->ptr_ref_mut(unsafe).get()) = *(rhs.ptr_ref().get());
			} else {
				*this = BoxIncomplete(rhs);
			}
		}
		return *this;
	}

	VEG_INLINE ~BoxIncomplete() {
		static_assert(VEG_CONCEPT(nothrow_destructible<T>), ".");
		static_assert(VEG_CONCEPT(alloc::nothrow_dealloc<A>), ".");
		auto ptr = this->ptr_ref().get();
		if (ptr != nullptr) {
			mem::destroy_at(ptr);
		}
	}
};

VEG_TEMPLATE(
		(typename LT, typename RT, typename LA, typename RA),
		requires(VEG_CONCEPT(eq<LT, RT>)),
		VEG_NODISCARD VEG_INLINE constexpr auto
		operator==,
		(lhs, BoxIncomplete<LT, LA> const&),
		(rhs, BoxIncomplete<RT, RA> const&))
VEG_NOEXCEPT_IF(VEG_CONCEPT(nothrow_eq<LT, RT>))->bool {
	return (lhs.ptr_ref().get() == nullptr || rhs.ptr_ref().get() == nullptr)
	           ? (lhs.ptr_ref().get() == nullptr &&
	              rhs.ptr_ref().get() == nullptr)
	           : (*(lhs.ptr_ref().get()) == *(rhs.ptr_ref().get()));
}
} // namespace mem
template <typename T, typename A = mem::SystemAlloc>
struct Box
		: meta::conditional_t<
					VEG_CONCEPT(nothrow_copyable<T>) && VEG_CONCEPT(nothrow_copyable<A>),
					internal::EmptyI<13>,
					internal::NoCopyCtor>,
			meta::conditional_t<
					VEG_CONCEPT(nothrow_copy_assignable<T>) &&
							VEG_CONCEPT(nothrow_copy_assignable<A>),
					internal::EmptyI<12>,
					internal::NoCopyAssign>,

			mem::BoxIncomplete<T, A> {
	using mem::BoxIncomplete<T, A>::BoxIncomplete;
};

namespace internal {
namespace _mem {
struct OrdBoxI {
	VEG_TEMPLATE(
			(typename LT, typename RT, typename LA, typename RA),
			requires(VEG_CONCEPT(ord<LT, RT>)),
			VEG_NODISCARD VEG_INLINE static VEG_CPP14(constexpr) auto cmp,
			(lhs, Ref<mem::BoxIncomplete<LT, LA>>),
			(rhs, Ref<mem::BoxIncomplete<RT, RA>>))
	VEG_NOEXCEPT_IF(VEG_CONCEPT(nothrow_ord<LT, RT>))->cmp::Ordering {
		if (lhs.get().ptr_ref().get() == nullptr &&
		    rhs.get().ptr_ref().get() == nullptr) {
			return cmp::Ordering::equal;
		}
		if (lhs.get().ptr_ref().get() == nullptr) {
			return cmp::Ordering::less;
		}
		if (rhs.get().ptr_ref().get() == nullptr) {
			return cmp::Ordering::greater;
		}
		return cmp::cmp( //
				ref(*lhs.get().ptr_ref().get()),
				ref(*rhs.get().ptr_ref().get()));
	}
};
struct OrdBox {
	VEG_TEMPLATE(
			(typename LT, typename RT, typename LA, typename RA),
			requires(VEG_CONCEPT(ord<LT, RT>)),
			VEG_NODISCARD VEG_INLINE static VEG_CPP14(constexpr) auto cmp,
			(lhs, Ref<Box<LT, LA>>),
			(rhs, Ref<Box<RT, RA>>))
	VEG_NOEXCEPT_IF(VEG_CONCEPT(nothrow_ord<LT, RT>))->cmp::Ordering {
		return OrdBoxI::cmp( //
				ref(static_cast<mem::BoxIncomplete<LT, LA> const&>(lhs.get())),
				ref(static_cast<mem::BoxIncomplete<RT, RA> const&>(rhs.get())));
	}
};
struct DbgBoxI {
	template <typename T>
	static void to_string_impl(fmt::BufferMut out, T const* ptr) noexcept {
		out.append_literal("[");
		if (ptr != nullptr) {
			fmt::dbg_to(VEG_FWD(out), ref(*ptr));
		}
		out.append_literal("]");
	}

	template <typename T, typename A>
	static void
	to_string(fmt::BufferMut out, Ref<mem::BoxIncomplete<T, A>> r) noexcept {
		to_string_impl(VEG_FWD(out), r.get().ptr_ref().get());
	}
};
struct DbgBox {
	template <typename T, typename A>
	static void to_string(fmt::BufferMut out, Ref<Box<T, A>> r) noexcept {
		DbgBoxI::to_string(
				VEG_FWD(out),
				ref(static_cast<mem::BoxIncomplete<T, A> const&>(r.get())));
	}
};
} // namespace _mem
} // namespace internal

template <typename T, typename A>
struct cpo::is_trivially_relocatable<mem::BoxIncomplete<T, A>>
		: cpo::is_trivially_relocatable<A> {};
template <typename T, typename A>
struct cpo::is_trivially_constructible<mem::BoxIncomplete<T, A>>
		: cpo::is_trivially_constructible<A> {};

template <typename T, typename A>
struct cpo::is_trivially_relocatable<Box<T, A>>
		: cpo::is_trivially_relocatable<A> {};
template <typename T, typename A>
struct cpo::is_trivially_constructible<Box<T, A>>
		: cpo::is_trivially_constructible<A> {};

template <typename LT, typename RT, typename LA, typename RA>
struct cmp::Ord<mem::BoxIncomplete<LT, LA>, mem::BoxIncomplete<RT, RA>>
		: internal::_mem::OrdBoxI {};
template <typename LT, typename RT, typename LA, typename RA>
struct cmp::Ord<Box<LT, LA>, Box<RT, RA>> : internal::_mem::OrdBox {};

template <typename T, typename A>
struct fmt::Debug<mem::BoxIncomplete<T, A>> : internal::_mem::DbgBoxI {};
template <typename T, typename A>
struct fmt::Debug<Box<T, A>> : internal::_mem::DbgBox {};

namespace nb {
struct box {
	template <typename T>
	VEG_INLINE auto operator()(T val) const
			VEG_NOEXCEPT_IF(VEG_CONCEPT(nothrow_movable<T>)) -> Box<T> {
		return {
				from_raw_parts,
				unsafe,
				internal::DefaultFn<mem::SystemAlloc>{},
				true,
				internal::MoveFn<T>{VEG_FWD(val)},
		};
	}
};
} // namespace nb
VEG_NIEBLOID(box);
} // namespace veg

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_BOX_HPP_HUJDLY6PS */