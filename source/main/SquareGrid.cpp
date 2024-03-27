#include "SquareGrid.hpp"

#include "Cast.hpp"

using namespace Project;

SquareGrid::SquareGrid(CommonInt const rowCount, CommonInt const columnCount):
    flatData(rowCount * columnCount), rowCount{rowCount}, columnCount{columnCount}
{
    if (rowCount < 0)
        throw std::invalid_argument("row count " + std::to_string(rowCount) + " cannpt be negative");

    if (columnCount < 0)
        throw std::invalid_argument("column count " + std::to_string(columnCount) + " cannot be negative");
}

CommonInt SquareGrid::getRowCount() const { return rowCount; }
CommonInt SquareGrid::getColumnCount() const { return columnCount; }
auto SquareGrid::getFlatData() const -> Table const & { return flatData; }

auto SquareGrid::at(CommonInt const row, CommonInt const column) -> Tile & { return flatData.at(row * columnCount + column); }
auto SquareGrid::at(CommonInt const row, CommonInt const column) const -> Tile const & { return flatData.at(row * columnCount + column); }

std::string SquareGrid::toString(char const wallSymbol, char const emptySymbol) const {
    std::stringstream buffer;

    for (CommonInt row{0}; row < rowCount; ++row) {
        for (CommonInt column{0}; column < columnCount; ++column) {
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
