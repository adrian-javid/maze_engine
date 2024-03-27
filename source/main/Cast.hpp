#ifndef Cast_hpp
#define Cast_hpp

#include "CommonInt.hpp"
#include <cstddef>
#include <limits>

namespace Project::Cast {

    constexpr auto toInt(std::size_t const value) -> CommonInt {
        if (value <= static_cast<std::size_t>(std::numeric_limits<CommonInt>::max()))
            return static_cast<CommonInt>(value);
        else
            return std::numeric_limits<CommonInt>::max();
    }

}

#endif
