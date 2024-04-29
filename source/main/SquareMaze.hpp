#ifndef SquareMaze_hpp
#define SquareMaze_hpp

// parent class
#include "Maze.hpp"

// grid to string
#include <string>
#include <sstream>

// table of tiles
#include <vector>

#include "Vector2.hpp"

namespace Project {
  class SquareMaze;
  std::ostream &operator<<(std::ostream &outputStream, SquareMaze const &squareGrid);
}

class Project::SquareMaze : public Project::Maze {

  public:

    static constexpr Vector2 northOffset = {-1,  0};
    static constexpr Vector2 southOffset = {+1,  0};
    static constexpr Vector2 eastOffset  = { 0, +1};
    static constexpr Vector2 westOffset  = { 0, -1};

  private:

    std::vector<Tile> table;
    static_assert(not std::is_same_v<std::vector<Tile>, std::vector<bool>>);
    int rowCount;
    int columnCount;

  public:

    explicit SquareMaze();
    explicit SquareMaze(int const rowCount, int const columnCount, Tile const tileFillValue=emptyTile);
    int getRowCount() const;
    int getColumnCount() const;
    std::vector<Tile> const &getTable() const;

    std::size_t getTileCount() const override;

    Tile &at(Vector2 const &) override;
    Tile const &at(Vector2 const &) const override;

    void forEachKey(std::function<void(Vector2 const &)> const &) const override;

    void forEachPrincipalDirection(std::function<void(Direction const)> const &) const override;

    TileAdjacency checkAdjacent(Vector2, Direction const) const override;

    constexpr Vector2 wrapKey(Vector2 const &key) const { return key.wrap(rowCount, columnCount); }

    constexpr std::size_t getFlatIndex(Vector2 const &key) const {
      int const flatIndex{/* row */key.value1 * columnCount + /* column */key.value2};
      return static_cast<std::size_t>(flatIndex);
    }

    std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

    bool isInBounds(Vector2 const &key) const override;
};

#endif
