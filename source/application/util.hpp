#ifndef Application_Util_hpp
#define Application_Util_hpp

#include <cmath>
#include <cassert>
#include <type_traits>
#include <functional>
#include <string>
#include "maze_engine/auxiliary.hpp"
#include "macros.hpp"
#include <iostream>

namespace App::Util {

	FORCE_INLINE inline
	double percentageWrap(double const value) { return MazeEngine::Aux::wrap(value, 1.00); }

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

	/*
		Print message with trailing newline character, then exit with failure code.
	*/
	template<typename... ParamsT>
	[[noreturn]] FORCE_INLINE inline
	void errOut(ParamsT &&...args) {
		(std::cerr << ... << std::forward<ParamsT>(args)) << '\n';
		std::exit(EXIT_FAILURE);
	}
}

#endif
