#include "Util.hpp"

using namespace Project;

double Util::wrapValue(double value, double const upperBound, double const lowerBound) {
    assert(upperBound != 0);
    assert(lowerBound < upperBound);

    value = std::fmod(value, upperBound);

    if (value < lowerBound) value += upperBound;
    
    if (value >= upperBound) value = lowerBound;

    return value;
}
