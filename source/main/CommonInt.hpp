#ifndef CommonInt_hpp
#define CommonInt_hpp

#include <cstdint>

namespace Project {

    /// @brief Fastest signed integral type that is at least 32 bits.
    using CommonInt = std::int_fast32_t;

    constexpr std::size_t commonIntMaxLimit = static_cast<std::size_t>(std::numeric_limits<CommonInt>::max());

}

#endif
