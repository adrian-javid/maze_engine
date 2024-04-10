#ifndef Util_hpp
#define Util_hpp

#include <cmath>
#include <cassert>

namespace Project::Util {
    double wrapValue(double value, double const upperBound, double const lowerBound=0.0);

    constexpr double linearInterpolation(double const percentage, double const start, double const end) {
        assert(percentage >= 0.0);
        assert(percentage <= 1.0);
        return start * (1.0 - percentage) + end * percentage;
    }
}

#endif
