#include "cast.hpp"

#include <limits>

constexpr auto Project::Cast::toInt(size_t const value) -> CommonInt {
    if (value <= static_cast<size_t>(std::numeric_limits<CommonInt>::max()))
        return static_cast<CommonInt>(value);
    else
        return std::numeric_limits<CommonInt>::max();
}
