#ifndef Vector2_hpp
#define Vector2_hpp

#include "Util.hpp"

#include <functional>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <array>

namespace Project { struct Vector2; }

/**
 * @brief Ordered pair of `int`. Can represent the identifier of a tile in a grid.
 */
struct Project::Vector2 {

    static Vector2 const squareNorth;
    static Vector2 const squareSouth;
    static Vector2 const squareEast;
    static Vector2 const squareWest;

    static Vector2 const hexagonNorthWest;
    static Vector2 const hexagonNorthEast;
    static Vector2 const hexagonEast;
    static Vector2 const hexagonSouthEast;
    static Vector2 const hexagonSouthWest;
    static Vector2 const hexagonWest;

    int value1;
    int value2;

    constexpr int ThirdAxis() const { return -value1 - value2; }

    constexpr Vector2 hexagonalRotate(int const indexDegree60) const {
        std::array<int, 3> vector{};
        int const signFactor = indexDegree60 % 2 == 0 ? 1 : -1;
        vector[Util::wrapValue(0 - indexDegree60, 3)] = this->value1;
        vector[Util::wrapValue(1 - indexDegree60, 3)] = this->value2;
        vector[Util::wrapValue(2 - indexDegree60, 3)] = this->ThirdAxis();
        return Vector2(vector[0], vector[1]) * signFactor;
    }

    /// @brief Create a vector with `row` `0` and `col` `0`.
    constexpr Vector2(): Vector2(0, 0) {}

    /**
     * @brief Create a vector with specified values.
     * @param row assign `row`
     * @param column assign `col`
    */
    constexpr Vector2(int row, int column): value1{row}, value2{column} {}

    constexpr Vector2 operator+(Vector2 const &vector) const {
        return {value1 + vector.value1, value2 + vector.value2};
    }

    constexpr Vector2 &operator+=(Vector2 const &vector) {
        return (*this = *this + vector);
    }

    constexpr Vector2 operator-(Vector2 const &vector) const {
        return {value1 - vector.value1, value2 - vector.value2};
    }

    constexpr Vector2 &operator-=(Vector2 const &vector) {
        return (*this = *this - vector);
    }

    constexpr bool operator==(Vector2 const &vector) const { return value1 == vector.value1 && value2 == vector.value2; }

    constexpr bool operator!=(Vector2 const &vector) const { return not(*this == vector); }

    constexpr bool operator<(Vector2 const &vector) const {
        if (value1 != vector.value1)
            return value1 < vector.value1;
        else
            return value2 < vector.value2;
    }

    constexpr bool operator>=(Vector2 const &vector) const { return not(*this < vector); }

    constexpr bool operator>(Vector2 const &vector) const { return vector < *this; }

    constexpr bool operator<=(Vector2 const &vector) const { return not(vector < *this); }

    constexpr Vector2 operator*(int const scalar) const {
        return Vector2(this->value1 * scalar, this->value2 * scalar);
    }

    constexpr Vector2 wrap(int const rowCount, int const columnCount) const {
        assert(rowCount > 0);
        assert(columnCount > 0);
        Vector2 vector(value1 % rowCount, value2 % columnCount);
        if (vector.value1 < 0) vector.value1 += rowCount;
        if (vector.value2 < 0) vector.value2 += columnCount;
        return vector;
    }

    struct Hash { std::size_t operator()(Vector2 const &vector) const noexcept; };

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
