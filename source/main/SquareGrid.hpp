#ifndef SquareGrid_hpp
#define SquareGrid_hpp

// parent class
#include "Grid.hpp"

// grid to string
#include <string>
#include <sstream>

// table of tiles
#include <vector>

namespace Project {
  class SquareGrid;
  std::ostream &operator<<(std::ostream &outputStream, SquareGrid const &squareGrid);
}

class Project::SquareGrid : public Project::Grid {

  public:

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

    Tile &at(CommonInt const row, CommonInt const column) override;
    Tile const &at(CommonInt const row, CommonInt const column) const override;

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

    void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const override;
};

#endif
