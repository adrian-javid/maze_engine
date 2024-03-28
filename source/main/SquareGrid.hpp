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
    int rowCount;
    int columnCount;

  public:

    explicit SquareGrid(int const rowCount=0, int const columnCount=0);
    int RowCount() const;
    int ColumnCount() const;
    Table const &getFlatData() const;

    Tile &at(int const row, int const column) override;
    Tile const &at(int const row, int const column) const override;

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

    void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const override;
};

#endif
