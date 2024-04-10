#ifndef Util_hpp
#define Util_hpp

#include <cmath>
#include <cassert>
#include <type_traits>

namespace Project::Util {

    template<class T>
    [[nodiscard]] constexpr auto abs(T const& x) noexcept {
        static_assert(std::is_arithmetic_v<T>);
        return x < 0 ? -x : x;
    }

    constexpr int wrapValue(int value, int const upperBound) {
        value %= upperBound;
        if (value < 0) value += upperBound;
        return value;
    }

    double wrapValue(double value, double const upperBound);

    constexpr double linearInterpolation(double const percentage, double const start, double const end) {
        assert(percentage >= 0.0);
        assert(percentage <= 1.0);
        return start * (1.0 - percentage) + end * percentage;
    }
}

#endif
