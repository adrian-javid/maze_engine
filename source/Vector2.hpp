#ifndef Vector2_hpp
#define Vector2_hpp

#include <functional>
#include <string>

namespace csm4880 { struct Vector2; }

struct csm4880::Vector2 {
    int row;
    int col;

    constexpr Vector2();
    constexpr Vector2(int row, int column);

    Vector2 operator+(Vector2 const&) const;
    Vector2 operator-(Vector2 const&) const;

    bool operator==(Vector2 const&) const;
    bool operator!=(Vector2 const&) const;

    Vector2 wrap(int const rowCount, int const columnCount) const;

    struct Hash { size_t operator()(Vector2 const &vector) const noexcept; };

    std::string toString() const;

};

namespace csm4880 {
    extern Vector2 const NORTH;
    extern Vector2 const SOUTH;
    extern Vector2 const EAST;
    extern Vector2 const WEST;

    std::ostream& operator<<(std::ostream &outputStream, Vector2 const &vector);
}

#endif
