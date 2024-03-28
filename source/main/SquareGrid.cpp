#include "SquareGrid.hpp"

using namespace Project;

SquareGrid::SquareGrid(int const rowCount, int const columnCount):
    flatData([rowCount, columnCount]() constexpr -> std::size_t {
        if (rowCount < 0)
            throw std::invalid_argument("row count " + std::to_string(rowCount) + " should not be negative");

        if (columnCount < 0)
            throw std::invalid_argument("column count " + std::to_string(columnCount) + " should not be negative");

        return static_cast<std::size_t>(rowCount) * static_cast<std::size_t>(columnCount);
    }()), rowCount{rowCount}, columnCount{columnCount}
{}

int SquareGrid::RowCount() const { return rowCount; }
int SquareGrid::ColumnCount() const { return columnCount; }
auto SquareGrid::FlatData() const -> Table const & { return flatData; }

auto SquareGrid::at(int const row, int const column) -> Tile & {
    return flatData.at(getFlatIndex(row, column));
}

auto SquareGrid::at(int const row, int const column) const -> Tile const & {
    return flatData.at(getFlatIndex(row, column));
}

std::string SquareGrid::toString(char const wallSymbol, char const emptySymbol) const {
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

void SquareGrid::forNeighbor(Vector2 const &tileKey, std::function<void(Vector2 const &)> operate) const {
    operate((tileKey + Vector2::north).wrap(rowCount, columnCount));
    operate((tileKey + Vector2::south).wrap(rowCount, columnCount));
    operate((tileKey + Vector2::east ).wrap(rowCount, columnCount));
    operate((tileKey + Vector2::west ).wrap(rowCount, columnCount));
}

std::ostream &Project::operator<<(std::ostream &outputStream, SquareGrid const &squareGrid) {
    outputStream << squareGrid.toString(); return outputStream;
}
