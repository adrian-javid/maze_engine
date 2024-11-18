#ifndef Application_Timer_hpp
#define Application_Timer_hpp

#include "delta_time.hpp"

namespace App { struct Timer; }

struct App::Timer {
	static constexpr UnsignedMilliseconds oneSecond{1'000u};

	UnsignedMilliseconds accumulatedTime, interval;

	[[nodiscard]] constexpr explicit Timer(UnsignedMilliseconds const initInterval):
		accumulatedTime{0u}, interval{initInterval}
	{}

	enum struct Modifier : std::uint_least8_t { none = 0u, compensateLostTime = 1u };

	/*
		Return `true` if spanned interval, otherwise `false`.
	*/
	template <Modifier modifier=Modifier::none>
	constexpr bool update() {
		accumulatedTime += getDeltaTime();

		if (accumulatedTime >= interval) {
			if constexpr (modifier == Modifier::compensateLostTime) {
				accumulatedTime -= interval;
			} else {
				accumulatedTime = 0u;
			}
			return true;
		}

		return false;
	}
};

#endif
