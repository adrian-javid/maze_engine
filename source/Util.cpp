#include "Util.hpp"

using namespace Project;

double Util::wrapValue(double value, double const upperBound) {
    constexpr double zero{0.0};

    assert(upperBound != 0);
    assert(zero < upperBound);

    value = std::fmod(value, upperBound);

    if (value < zero) value += upperBound;
    
    if (value >= upperBound) value = zero;

    return value;
}
