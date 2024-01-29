#include "Vector2.hpp"
#include <sstream>

using csm4880::Vector2;


Vector2 const csm4880::NORTH =  {-1,  0};
Vector2 const csm4880::SOUTH =  { 1,  0};
Vector2 const csm4880::EAST  =  { 0,  1};
Vector2 const csm4880::WEST  =  { 0, -1};

constexpr Vector2::Vector2(): Vector2(0, 0) {}
constexpr Vector2::Vector2(int row, int column): row{row}, col{column} {}

Vector2 Vector2::operator+(Vector2 const &vector) const { return {row + vector.row, col + vector.col}; }
Vector2 Vector2::operator-(Vector2 const &vector) const { return {row - vector.row, col - vector.col}; }

bool Vector2::operator==(Vector2 const &vector) const { return row == vector.row && col == vector.col; }
bool Vector2::operator!=(Vector2 const &vector) const { return not(*this == vector); }

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

std::ostream& csm4880::operator<<(std::ostream &outputStream, Vector2 const &vector) {
    outputStream << vector.toString(); return outputStream;
}
