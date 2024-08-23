#ifndef Application_Timer_hpp
#define Application_Timer_hpp

#include "delta_time.hpp"

namespace App { class Timer; }

class App::Timer {
	public:

		using UnsignedMilliseconds = Uint64;

		static constexpr UnsignedMilliseconds oneSecond{1'000u};

		UnsignedMilliseconds accumulatedTime, interval;

	[[nodiscard]] explicit Timer(UnsignedMilliseconds const initInterval):
		accumulatedTime{0u}, interval{initInterval}
	{}

	/*
		Return `true` if spanned interval, otherwise `false`.
	*/
	inline bool update() {
		accumulatedTime += getDeltaTime();

		if (accumulatedTime >= interval) {
			accumulatedTime -= interval;
			return true;
		}

		return false;
	}
};

#endif
