#ifndef Application_LinearInterpolation_hpp
#define Application_LinearInterpolation_hpp

#include <cmath>
#include <cassert>
#include <type_traits>
#include <functional>
#include <string>
#include "maze_engine/auxiliary.hpp"
#include "macros.hpp"
#include <iostream>

namespace App {

	constexpr float linearInterpolation(float const percentage, float const start, float const end) {
		assert(percentage >= 0.0f);
		assert(percentage <= 1.0f);
		return start * (1.0f - percentage) + end * percentage;
	}

	constexpr double linearInterpolation(double const percentage, double const start, double const end) {
		assert(percentage >= 0.0);
		assert(percentage <= 1.0);
		return start * (1.0 - percentage) + end * percentage;
	}
}

#endif
