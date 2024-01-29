#ifndef Vector2_hpp
#define Vector2_hpp

#include <functional>

namespace csm4880 { struct Vector2; }

struct csm4880::Vector2 {
    size_t row;

    size_t col;

    Vector2 wrap(size_t const rowCount, size_t const columnCount) const;

    struct Hash { size_t operator()(Vector2 const &vector) const noexcept; };
};

#endif
