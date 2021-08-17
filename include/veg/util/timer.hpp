#ifndef VEG_TIMER_HPP_2SVNQDV6S
#define VEG_TIMER_HPP_2SVNQDV6S

#include "veg/internal/typedefs.hpp"
#include "veg/type_traits/invocable.hpp"
#include "veg/util/defer.hpp"
#include <cstdio>
#include "veg/internal/prologue.hpp"

namespace veg {
namespace abi {
inline namespace VEG_ABI_VERSION {
namespace time {
auto monotonic_nanoseconds_since_epoch() VEG_ALWAYS_NOEXCEPT -> i64;
void log_elapsed_time(i64 duration, char const* msg, std::FILE* out)
		VEG_ALWAYS_NOEXCEPT;
} // namespace time
} // namespace VEG_ABI_VERSION
} // namespace abi

namespace internal {
namespace time_ {
template <typename Fn>
struct RaiiTimerWrapper {
	struct raw_parts {
		i64 begin;
		Fn fn;
	} self;

	RaiiTimerWrapper(Fn fn) VEG_NOEXCEPT
			: self{abi::time::monotonic_nanoseconds_since_epoch(), VEG_FWD(fn)} {}

	void operator()()
			VEG_NOEXCEPT_IF(VEG_CONCEPT(nothrow_fn_once<Fn, void, i64>)) {
		i64 time_delta =
				i64(abi::time::monotonic_nanoseconds_since_epoch() - self.begin);
		VEG_FWD(self.fn)(time_delta);
	}
};
} // namespace time_
} // namespace internal

namespace time {
namespace nb {
struct monotonic_nanoseconds_since_epoch {
	auto operator()() const VEG_NOEXCEPT -> i64 {
		return abi::time::monotonic_nanoseconds_since_epoch();
	}
};
} // namespace nb
VEG_NIEBLOID(monotonic_nanoseconds_since_epoch);

struct LogElapsedTime {
	explicit LogElapsedTime(char const* _msg = "", std::FILE* _out = stdout)
			VEG_ALWAYS_NOEXCEPT : msg{_msg},
														out{_out} {}

	char const* msg;
	std::FILE* out;

	void operator()(i64 duration) const VEG_ALWAYS_NOEXCEPT {
		abi::time::log_elapsed_time(duration, msg, out);
	}
};

template <typename Fn>
struct RaiiTimer : Defer<internal::time_::RaiiTimerWrapper<Fn>> {
	VEG_CHECK_CONCEPT(fn_once<Fn, void, i64>);
	using Defer<internal::time_::RaiiTimerWrapper<Fn>>::Defer;
	using Defer<internal::time_::RaiiTimerWrapper<Fn>>::fn;
};

namespace nb {
struct raii_timer {
	VEG_TEMPLATE(
			typename Fn,
			requires(VEG_CONCEPT(fn_once<Fn, void, i64>) && VEG_CONCEPT(movable<Fn>)),
			auto
			operator(),
			(fn, Fn&&))
	const VEG_NOEXCEPT_IF(VEG_CONCEPT(nothrow_movable<Fn>))->time::RaiiTimer<Fn> {
		return {VEG_FWD(fn)};
	}
};
} // namespace nb
VEG_NIEBLOID(raii_timer);
} // namespace time
} // namespace veg

#include "veg/internal/epilogue.hpp"
#endif /* end of include guard VEG_TIMER_HPP_2SVNQDV6S */
