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

int SquareMaze::RowCount() const { return rowCount; }
int SquareMaze::ColumnCount() const { return columnCount; }
auto SquareMaze::FlatData() const -> Table const & { return table; }

auto SquareMaze::at(int const row, int const column) -> Tile & {
    return table.at(getFlatIndex(row, column));
}

auto SquareMaze::at(int const row, int const column) const -> Tile const & {
    return table.at(getFlatIndex(row, column));
}

std::string SquareMaze::toString(char const wallSymbol, char const emptySymbol) const {
    std::stringstream buffer;

    for (int row{0}; row < rowCount; ++row) {
        for (int column{0}; column < columnCount; ++column) {
            Tile const tile = at(row, column);
            char const symbol = (tile ? wallSymbol : emptySymbol);
            buffer << ' ' << symbol;
        }
        buffer << ' ' << '\n';
    }

    return buffer.str();
}

void SquareMaze::forNeighbor(Vector2 const &tileKey, std::function<void(Vector2 const &)> operate) const {
    operate((tileKey + Vector2::squareNorth).wrap(rowCount, columnCount));
    operate((tileKey + Vector2::squareSouth).wrap(rowCount, columnCount));
    operate((tileKey + Vector2::squareEast ).wrap(rowCount, columnCount));
    operate((tileKey + Vector2::squareWest ).wrap(rowCount, columnCount));
}

bool SquareMaze::isOpen(int const row, int const column, Direction const direction) const {
    Tile const tile{at(row, column)};

    switch (direction) {
        case Direction::north:
            return tile & SquareMaze::northWall;
        case Direction::south:
            return at(row + Vector2::squareSouth.value1, column + Vector2::squareSouth.value2) & SquareMaze::northWall;
        case Direction::east:
            return tile & SquareMaze::eastWall;
        case Direction::west:
            return at(row + Vector2::squareWest.value1, column + Vector2::squareWest.value2) & SquareMaze::eastWall;
        default:
            return false;
    }
}

std::ostream &Project::operator<<(std::ostream &outputStream, SquareMaze const &squareGrid) {
    outputStream << squareGrid.toString(); return outputStream;
}
