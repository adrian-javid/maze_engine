#ifndef Application_DeltaTime_hpp
#define Application_DeltaTime_hpp

#include "simple_directmedia_layer.hpp"

namespace App {

	using UnsignedMilliseconds = decltype(SDL_GetTicks64());
	static_assert(std::is_integral_v<UnsignedMilliseconds>);
	static_assert(std::is_unsigned_v<UnsignedMilliseconds>);

	// Get change in time since last main loop iteration in milliseconds.
	[[nodiscard]] UnsignedMilliseconds getDeltaTime();
}

#endif
