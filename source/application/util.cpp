#include "util.hpp"

double App::Util::wrapValue(double value, double const upperBound) {
	static constexpr double zero{0.0};

	assert(upperBound != 0);
	assert(zero < upperBound);

	value = std::fmod(value, upperBound);

	if (value < zero) value += upperBound;

	if (value >= upperBound) value = zero;

	return value;
}
