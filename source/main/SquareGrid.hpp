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
    CommonInt rowCount;
    CommonInt columnCount;

  public:

    explicit SquareGrid(CommonInt const rowCount=0, CommonInt const columnCount=0);
    CommonInt getRowCount() const;
    CommonInt getColumnCount() const;
    Table const &getFlatData() const;

    Tile &at(CommonInt const row, CommonInt const column) override;
    Tile const &at(CommonInt const row, CommonInt const column) const override;

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

    void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const override;
};

#endif
