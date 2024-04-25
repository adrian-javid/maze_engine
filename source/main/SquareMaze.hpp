#ifndef SquareMaze_hpp
#define SquareMaze_hpp

// parent class
#include "Maze.hpp"

// grid to string
#include <string>
#include <sstream>

// table of tiles
#include <vector>

namespace Project {
  class SquareMaze;
  std::ostream &operator<<(std::ostream &outputStream, SquareMaze const &squareGrid);
}

class Project::SquareMaze : public Project::Maze {

  public:

    using Table = std::vector<Tile>;
    static_assert(not std::is_same_v<Table, std::vector<bool>>);

  private:

    Table table;
    int rowCount;
    int columnCount;

  public:

    explicit SquareMaze(int const rowCount=0, int const columnCount=0);
    int RowCount() const;
    int ColumnCount() const;
    Table const &FlatData() const;

    Tile &at(int const row, int const column) override;
    Tile const &at(int const row, int const column) const override;

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

    void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const override;

    bool isOpen(int const row, int const column, Direction const) const override;

    constexpr std::size_t getFlatIndex(int row, int column) const {
      row = Util::wrapValue(row, rowCount);
      column = Util::wrapValue(column, columnCount);

      int const flatIndex{row * columnCount + column};

      return static_cast<std::size_t>(flatIndex);
    }
};

#endif
