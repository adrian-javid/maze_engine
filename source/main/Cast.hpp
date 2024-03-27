#ifndef Cast_hpp
#define Cast_hpp

#include "CommonInt.hpp"
#include <cstddef>
#include <limits>
#include <optional>

namespace Project::Cast {

    constexpr auto toInt(std::size_t const value) -> std::optional<CommonInt> {
        if (value <= commonIntMaxLimit)
            return static_cast<CommonInt>(value);
        else
            return std::nullopt;
    }

}

#endif
