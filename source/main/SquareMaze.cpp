#include "SquareMaze.hpp"

using namespace Project;

SquareMaze::SquareMaze(int const rowCount, int const columnCount):
    table([rowCount, columnCount]() constexpr -> std::size_t {
        if (rowCount < 0)
            throw std::invalid_argument("row count " + std::to_string(rowCount) + " should not be negative");

        if (columnCount < 0)
            throw std::invalid_argument("column count " + std::to_string(columnCount) + " should not be negative");

        return static_cast<std::size_t>(rowCount) * static_cast<std::size_t>(columnCount);
    }()), rowCount{rowCount}, columnCount{columnCount}
{}

int SquareMaze::getRowCount() const { return rowCount; }
int SquareMaze::getColumnCount() const { return columnCount; }
auto SquareMaze::getTable() const -> std::vector<Tile> const & { return table; }

std::size_t SquareMaze::getTileCount() const { return table.size(); }

auto SquareMaze::at(Vector2 const &tileKey) -> Tile & {
    return table.at(getFlatIndex(tileKey));
}

auto SquareMaze::at(Vector2 const &tileKey) const -> Tile const & {
    return table.at(getFlatIndex(tileKey));
}

std::string SquareMaze::toString(char const wallSymbol, char const emptySymbol) const {
    std::stringstream buffer;

    for (Vector2 key(0); key.value1 < rowCount; ++key.value1) {
        for (key.value2 = 0; key.value2 < columnCount; ++key.value2) {
            Tile const tile = at(key);
            char const symbol = (tile ? wallSymbol : emptySymbol);
            buffer << ' ' << symbol;
        }
        buffer << ' ' << '\n';
    }

    return buffer.str();
}

void SquareMaze::forNeighbor(Vector2 const &tileKey, VertexAction const operate) const {
    if (not hasWall(tileKey, Direction::north)) operate((tileKey + Vector2::squareNorth).wrap(rowCount, columnCount));
    if (not hasWall(tileKey, Direction::south)) operate((tileKey + Vector2::squareSouth).wrap(rowCount, columnCount));
    if (not hasWall(tileKey, Direction::east )) operate((tileKey + Vector2::squareEast ).wrap(rowCount, columnCount));
    if (not hasWall(tileKey, Direction::west )) operate((tileKey + Vector2::squareWest ).wrap(rowCount, columnCount));
}

bool SquareMaze::hasWall(Vector2 const &tileKey, Direction const direction) const {
    switch (direction) {
        case Direction::north: return at(tileKey                       ) & SquareMaze::northWall;
        case Direction::east : return at(tileKey                       ) & SquareMaze::eastWall ;
        case Direction::south: return at(tileKey + Vector2::squareSouth) & SquareMaze::northWall;
        case Direction::west : return at(tileKey + Vector2::squareWest ) & SquareMaze::eastWall ;

        default:
            return false;
    }
}

std::ostream &Project::operator<<(std::ostream &outputStream, SquareMaze const &squareGrid) {
    outputStream << squareGrid.toString(); return outputStream;
}
