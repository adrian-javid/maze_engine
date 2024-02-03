#ifndef Vector2_hpp
#define Vector2_hpp

#include <functional>
#include <string>
#include <unordered_set>
#include <unordered_map>

namespace Project { struct Vector2; }

struct Project::Vector2 {

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

namespace Project {

    Vector2 constexpr Vector2_NORTH = {-1,  0};
    Vector2 constexpr Vector2_SOUTH = { 1,  0};
    Vector2 constexpr Vector2_EAST  = { 0,  1};
    Vector2 constexpr Vector2_WEST  = { 0, -1};

    std::ostream& operator<<(std::ostream &outputStream, Vector2 const &vector);

}

#endif
