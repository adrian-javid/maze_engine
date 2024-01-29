#ifndef Vector2_hpp
#define Vector2_hpp

#include <functional>

namespace csm4880 { struct Vector2; }

struct csm4880::Vector2 {
    size_t row;
    size_t col;

    struct Hash { size_t operator()(Vector2 const &vector) const noexcept; };
};

#endif
