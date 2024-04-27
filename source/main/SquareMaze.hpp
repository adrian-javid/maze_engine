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
    static_assert(not std::is_same_v<std::vector<Tile>, std::vector<bool>>);

  private:

    std::vector<Tile> table;
    int rowCount;
    int columnCount;

  public:

    explicit SquareMaze(int const rowCount=0, int const columnCount=0);
    int getRowCount() const;
    int getColumnCount() const;
    std::vector<Tile> const &getTable() const;

    std::size_t getTileCount() const override;

    Tile &at(Vector2 const &) override;
    Tile const &at(Vector2 const &) const override;

    void forEachTile(std::function<void(Vector2 const &, Tile const)> const &) const override;

    void forEachValidDirection(std::function<void(Direction const)> const &) const override;

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

    void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)> const &) const override;

    bool hasWall(Vector2 const &, Direction const) const override;

    constexpr std::size_t getFlatIndex(Vector2 const &tileKey) const {
      int const row = Util::wrapValue(tileKey.value1, rowCount);
      int const column = Util::wrapValue(tileKey.value2, columnCount);
      int const flatIndex{row * columnCount + column};
      return static_cast<std::size_t>(flatIndex);
    }
};

#endif
