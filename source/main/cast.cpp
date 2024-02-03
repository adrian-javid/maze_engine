#include "cast.hpp"

#include <limits>

int Project::cast::toInt(size_t const value) {
    if (value <= static_cast<size_t>(std::numeric_limits<int>::max()))
        return static_cast<int>(value);
    else
        return std::numeric_limits<int>::max();
}
