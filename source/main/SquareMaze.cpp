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
    return table.at(getFlatIndex(wrapKey(tileKey)));
}

auto SquareMaze::at(Vector2 const &tileKey) const -> Tile const & {
    return table.at(getFlatIndex(wrapKey(tileKey)));
}

void SquareMaze::forEachTile(std::function<void(Vector2 const &, Tile const)> const &forThisTile) const {
    for (Vector2 key(0); key.value1 < rowCount; ++key.value1)
        for (key.value2 = 0; key.value2 < columnCount; ++key.value2)
            forThisTile(key, table.at(getFlatIndex(key)));
}

void SquareMaze::forEachPrincipalDirection(std::function<void(Direction const)> const &forThisPrincipalDirection) const {
    forThisPrincipalDirection(north);
    forThisPrincipalDirection(east);
}

void SquareMaze::forEachValidDirection(std::function<void(Direction const)> const &forThisDirection) const {
    forThisDirection(north);
    forThisDirection(south);
    forThisDirection(east );
    forThisDirection(west );
}

auto SquareMaze::checkAdjacent(Vector2 key, Direction const direction) const -> TileAdjacency {
    switch (direction) {
        case Direction::north:
            return {wrapKey(key + Vector2::squareNorth), at(key) & north};
        case Direction::east :
            return {wrapKey(key + Vector2::squareEast), at(key) & east};

        case Direction::south:
            key = wrapKey(key + Vector2::squareSouth);
            return {key, table.at(getFlatIndex(key)) & north};
        case Direction::west :
            key = wrapKey(key + Vector2::squareWest);
            return {key, table.at(getFlatIndex(key)) & east};

        default:
            throw direction;
    }
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

std::ostream &Project::operator<<(std::ostream &outputStream, SquareMaze const &squareGrid) {
    outputStream << squareGrid.toString(); return outputStream;
}
