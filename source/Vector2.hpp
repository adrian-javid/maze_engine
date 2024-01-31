#ifndef Vector2_hpp
#define Vector2_hpp

#include <functional>
#include <string>
#include <unordered_set>
#include <unordered_map>

namespace csm4880 { struct Vector2; }

struct csm4880::Vector2 {
    static Vector2 const NORTH;
    static Vector2 const SOUTH;
    static Vector2 const EAST;
    static Vector2 const WEST;

    int row;
    int col;

    constexpr Vector2(): Vector2(0, 0) {}
    constexpr Vector2(int row, int column): row{row}, col{column} {}

    Vector2 operator+(Vector2 const &) const;
    Vector2 operator-(Vector2 const &) const;

    bool operator==(Vector2 const &) const;
    bool operator!=(Vector2 const &) const;

    bool operator<(Vector2 const &) const;
    bool operator>=(Vector2 const &) const;
    bool operator>(Vector2 const &) const;
    bool operator<=(Vector2 const &) const;

    Vector2 wrap(int const rowCount, int const columnCount) const;

    struct Hash { size_t operator()(Vector2 const &vector) const noexcept; };

    using HashSet = std::unordered_set<Vector2, Hash>;
    template<typename T> using HashMap = std::unordered_map<Vector2, T, Hash>;

    std::string toString() const;

};

namespace csm4880 {
    std::ostream& operator<<(std::ostream &outputStream, Vector2 const &vector);
}

#endif
