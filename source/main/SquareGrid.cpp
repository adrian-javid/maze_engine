#include "SquareGrid.hpp"

#include "Cast.hpp"

using Project::SquareGrid;

SquareGrid::SquareGrid(size_t const rowCount, size_t const columnCount):
    flatData(rowCount * columnCount), rowCount{rowCount}, columnCount{columnCount}
{}

size_t SquareGrid::getRowCount() const { return rowCount; }
size_t SquareGrid::getColumnCount() const { return columnCount; }
auto SquareGrid::getFlatData() const -> Table const & { return flatData; }

auto SquareGrid::at(CommonInt const row, CommonInt const column) -> Tile & { return flatData.at(row * columnCount + column); }
auto SquareGrid::at(CommonInt const row, CommonInt const column) const -> Tile const & { return flatData.at(row * columnCount + column); }

std::string SquareGrid::toString(char const wallSymbol, char const emptySymbol) const {
    std::stringstream buffer;

    for (size_t row{0}; row < rowCount; ++row) {
        for (size_t column{0}; column < columnCount; ++column) {
            Tile const tile = at(row, column);
            char const symbol = (tile ? wallSymbol : emptySymbol);
            buffer << ' ' << symbol;
        }
        buffer << ' ' << '\n';
    }

    return buffer.str();
}

void SquareGrid::forNeighbor(Vector2 const &tileKey, std::function<void(Vector2 const &)> operate) const {
    CommonInt const rowCountInt    = Cast::toInt(getRowCount   ());
    CommonInt const columnCountInt = Cast::toInt(getColumnCount());

    operate((tileKey + Vector2::north).wrap(rowCountInt, columnCountInt));
    operate((tileKey + Vector2::south).wrap(rowCountInt, columnCountInt));
    operate((tileKey + Vector2::east ).wrap(rowCountInt, columnCountInt));
    operate((tileKey + Vector2::west ).wrap(rowCountInt, columnCountInt));
}

std::ostream &Project::operator<<(std::ostream &outputStream, SquareGrid const &squareGrid) {
    outputStream << squareGrid.toString(); return outputStream;
}
