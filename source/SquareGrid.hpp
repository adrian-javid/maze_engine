#ifndef SquareGrid_hpp
#define SquareGrid_hpp

#include <string>
#include <sstream>
#include <vector>
#include <type_traits>

namespace csm4880 {
  class SquareGrid;
  std::ostream& operator<<(std::ostream &outputStream, SquareGrid const &squareGrid);
}

class csm4880::SquareGrid {

  public:

    using Tile = uint_fast8_t;
    using Table = std::vector<Tile>;
    static_assert(not std::is_same_v<Table, std::vector<bool>>);

    constexpr static Tile NONE = 0;
    constexpr static Tile WALL = 1 << 0;

  private:

    Table flatData;
    size_t rowCount;
    size_t columnCount;

  public:

    explicit SquareGrid(size_t const rowCount, size_t const columnCount);
    size_t getRowCount() const;
    size_t getColumnCount() const;
    Table const &getFlatData() const;

    Tile &at(size_t const row, size_t const column);
    Tile const &at(size_t const row, size_t const column) const;
    bool isWall(size_t const row, size_t const column) const;

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

};

#endif
