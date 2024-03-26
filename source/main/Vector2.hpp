#ifndef Vector2_hpp
#define Vector2_hpp

#include <functional>
#include <string>
#include <unordered_set>
#include <unordered_map>

#include "CommonInt.hpp"

namespace Project { struct Vector2; }

/**
 * @brief Ordered pair of `int`. Can represent the identifier of a tile in a grid.
 */
struct Project::Vector2 {

    static Vector2 const north;
    static Vector2 const south;
    static Vector2 const east;
    static Vector2 const west;

    CommonInt row;
    CommonInt col;

    /// @brief Create a vector with `row` `0` and `col` `0`.
    constexpr Vector2(): Vector2(0, 0) {}

    /**
     * @brief Create a vector with specified values.
     * @param row assign `row`
     * @param column assign `col`
    */
    constexpr Vector2(CommonInt row, CommonInt column): row{row}, col{column} {}

    Vector2 operator+(Vector2 const &) const;
    Vector2 operator-(Vector2 const &) const;

    bool operator==(Vector2 const &) const;
    bool operator!=(Vector2 const &) const;

    bool operator<(Vector2 const &) const;
    bool operator>=(Vector2 const &) const;
    bool operator>(Vector2 const &) const;
    bool operator<=(Vector2 const &) const;

    Vector2 wrap(CommonInt const rowCount, CommonInt const columnCount) const;

    struct Hash { size_t operator()(Vector2 const &vector) const noexcept; };

    /// @brief An unorderd set of `Vector2`.
    using HashSet = std::unordered_set<Vector2, Hash>;

    /**
     * @brief An unordered hash map from `Vector2` to `T`.
     * @tparam T value mapped from `Vector2`
     */
    template<typename T> using HashMap = std::unordered_map<Vector2, T, Hash>;

    std::string toString() const;
};

namespace Project {

    std::ostream& operator<<(std::ostream &outputStream, Vector2 const &vector);

}

#endif
