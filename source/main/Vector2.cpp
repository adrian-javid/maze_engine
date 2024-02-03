#include "Vector2.hpp"
#include <sstream>

using Project::Vector2;

Vector2 Vector2::operator+(Vector2 const &vector) const { return {row + vector.row, col + vector.col}; }
Vector2 Vector2::operator-(Vector2 const &vector) const { return {row - vector.row, col - vector.col}; }

bool Vector2::operator==(Vector2 const &vector) const { return row == vector.row && col == vector.col; }
bool Vector2::operator!=(Vector2 const &vector) const { return not(*this == vector); }

bool Vector2::operator<(Vector2 const &vector) const {
    if (row != vector.row)
        return row < vector.row;
    else
        return col < vector.col;
}

bool Vector2::operator>=(Vector2 const &vector) const { return not(*this < vector); }

bool Vector2::operator>(Vector2 const &vector) const { return vector < *this; }

bool Vector2::operator<=(Vector2 const &vector) const { return not(vector < *this); }

Vector2 Vector2::wrap(int const rowCount, int const columnCount) const {
    Vector2 vector(row % rowCount, col % columnCount);
    if (vector.row < 0) vector.row += rowCount;
    if (vector.col < 0) vector.col += columnCount;
    return vector;
}

size_t Vector2::Hash::operator()(Vector2 const &vector) const noexcept {
    static std::hash<size_t> constexpr hash;
    size_t hashRow = hash(vector.row);
    size_t hashCol = hash(vector.col);
    /*
        This is a copy of `boost::hash_combine`.
        https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
    */
    return hashRow ^ (hashCol + 0x9e3779b9 + (hashRow << 6) + (hashRow >> 2));
}

std::string Vector2::toString() const {
    std::stringstream buffer;
    buffer << '(' << row << ", " << col << ")";
    return buffer.str();
}

std::ostream& Project::operator<<(std::ostream &outputStream, Vector2 const &vector) {
    outputStream << vector.toString(); return outputStream;
}
