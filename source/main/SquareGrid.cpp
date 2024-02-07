#include "SquareGrid.hpp"

using Project::SquareGrid;

SquareGrid::SquareGrid(size_t const rowCount, size_t const columnCount):
    flatData(rowCount * columnCount), rowCount{rowCount}, columnCount{columnCount}
{}

size_t SquareGrid::getRowCount() const { return rowCount; }
size_t SquareGrid::getColumnCount() const { return columnCount; }
auto SquareGrid::getFlatData() const -> Table const & { return flatData; }

auto SquareGrid::at(Grid::Index const row, Grid::Index const column) -> Tile & { return flatData.at(row * columnCount + column); }
auto SquareGrid::at(Grid::Index const row, Grid::Index const column) const -> Tile const & { return flatData.at(row * columnCount + column); }

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

std::ostream& Project::operator<<(std::ostream &outputStream, SquareGrid const &squareGrid) {
    outputStream << squareGrid.toString(); return outputStream;
}
