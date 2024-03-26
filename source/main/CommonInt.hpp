#ifndef CommonInt_hpp
#define CommonInt_hpp

#include <cstdint>

namespace Project {
    /// @brief Fastest signed integral type that is at least 32 bits.
    using CommonInt = std::int_fast32_t;
}

#endif
