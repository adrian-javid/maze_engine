#include "Vector2.hpp"
#include <sstream>
#include <cassert>

using Project::Vector2;

Vector2 const Vector2::squareNorth(-1,  0);
Vector2 const Vector2::squareSouth(+1,  0);
Vector2 const Vector2::squareEast ( 0, +1);
Vector2 const Vector2::squareWest ( 0, -1);

Vector2 const Vector2::hexagonNorthWest( 0, -1);
Vector2 const Vector2::hexagonNorthEast(+1, -1);
Vector2 const Vector2::hexagonEast     (+1,  0);
Vector2 const Vector2::hexagonSouthEast( 0, +1);
Vector2 const Vector2::hexagonSouthWest(-1, +1);
Vector2 const Vector2::hexagonWest     (-1,  0);

Vector2 Vector2::operator+(Vector2 const &vector) const { return {value1 + vector.value1, value2 + vector.value2}; }
Vector2 Vector2::operator-(Vector2 const &vector) const { return {value1 - vector.value1, value2 - vector.value2}; }

bool Vector2::operator==(Vector2 const &vector) const { return value1 == vector.value1 && value2 == vector.value2; }
bool Vector2::operator!=(Vector2 const &vector) const { return not(*this == vector); }

bool Vector2::operator<(Vector2 const &vector) const {
    if (value1 != vector.value1)
        return value1 < vector.value1;
    else
        return value2 < vector.value2;
}

bool Vector2::operator>=(Vector2 const &vector) const { return not(*this < vector); }

bool Vector2::operator>(Vector2 const &vector) const { return vector < *this; }

bool Vector2::operator<=(Vector2 const &vector) const { return not(vector < *this); }

Vector2 Vector2::wrap(int const rowCount, int const columnCount) const {
    assert(rowCount > 0);
    assert(columnCount > 0);
    Vector2 vector(value1 % rowCount, value2 % columnCount);
    if (vector.value1 < 0) vector.value1 += rowCount;
    if (vector.value2 < 0) vector.value2 += columnCount;
    return vector;
}

std::size_t Vector2::Hash::operator()(Vector2 const &vector) const noexcept {
    static_assert(std::is_same_v<decltype(vector.value1), decltype(vector.value2)>);
    static constexpr std::hash<decltype(vector.value1)> hash;
    size_t const hashRow = hash(vector.value1);
    size_t const hashCol = hash(vector.value2);
    /*
        This is a copy of `boost::hash_combine`.
        https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
    */
    return hashRow ^ (hashCol + 0x9e3779b9 + (hashRow << 6) + (hashRow >> 2));
}

std::string Vector2::toString() const {
    std::stringstream buffer;
    buffer << '(' << value1 << ", " << value2 << ")";
    return buffer.str();
}

std::ostream& Project::operator<<(std::ostream &outputStream, Vector2 const &vector) {
    outputStream << vector.toString(); return outputStream;
}
