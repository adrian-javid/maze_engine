#ifndef SquareGrid_hpp
#define SquareGrid_hpp

#include <string>
#include <sstream>
#include <vector>
#include <type_traits>

namespace csm4880 { class SquareGrid; }

class csm4880::SquareGrid {

  public:

    using Tile = uint_fast8_t;
    using Table = std::vector<Tile>;
    static_assert(! std::is_same_v<Table, std::vector<bool>>);

    constexpr static Tile EMPTY = 1 >> 1;
    constexpr static Tile WALL  = 1 << 0;

  private:

    Table flatData;
    size_t rowCount;
    size_t columnCount;

  public:

    explicit SquareGrid(size_t const rowCount, size_t const columnCount):
      flatData(rowCount * columnCount), rowCount{rowCount}, columnCount{columnCount}
    {}

    size_t getRowCount() const { return rowCount; }
    size_t getColumnCount() const { return columnCount; }
    Table const &getFlatData() const { return flatData; }

    Tile &at(size_t const row, size_t const column) { return flatData.at(row * columnCount + column); }
    Tile const &at(size_t const row, size_t const column) const { return flatData.at(row * columnCount + column); }

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const {
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

    friend std::ostream& operator<<(std::ostream &outputStream, SquareGrid const &squareGrid) {
      outputStream << squareGrid.toString(); return outputStream;
    }

};

#endif
