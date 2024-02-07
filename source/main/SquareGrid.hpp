#ifndef SquareGrid_hpp
#define SquareGrid_hpp

#include <string>
#include <sstream>
#include <vector>
#include <type_traits>
#include "Grid.hpp"

namespace Project {
  class SquareGrid;
  std::ostream &operator<<(std::ostream &outputStream, SquareGrid const &squareGrid);
}

class Project::SquareGrid : public Project::Grid {

  public:

    using Tile = uint_fast8_t;
    using Table = std::vector<Tile>;
    static_assert(not std::is_same_v<Table, std::vector<bool>>);

  private:

    Table flatData;
    size_t rowCount;
    size_t columnCount;

  public:

    explicit SquareGrid(size_t const rowCount=0, size_t const columnCount=0);
    size_t getRowCount() const;
    size_t getColumnCount() const;
    Table const &getFlatData() const;

    Tile &at(size_t const row, size_t const column);
    Tile const &at(size_t const row, size_t const column) const;

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

};

#endif
