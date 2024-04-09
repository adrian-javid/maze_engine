#ifndef Util_hpp
#define Util_hpp

#include <cmath>
#include <cassert>

namespace Project::Util {
    constexpr double wrapValue(double value, double const upperBound, double const lowerBound=0.0) {
        assert(upperBound != 0);
        assert(lowerBound < upperBound);

        value = std::fmod(value, upperBound);

        if (value < lowerBound) value += upperBound;
        
        if (value >= upperBound) value = lowerBound;

        return value;
    }
}

#endif
