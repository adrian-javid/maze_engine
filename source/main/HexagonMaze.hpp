#ifndef HexagonMaze_hpp
#define HexagonMaze_hpp

#include "Maze.hpp"

#include <array>

namespace Project { class HexagonMaze; }

class Project::HexagonMaze : public Project::Maze {

  private:

    Vector2::HashMap<Maze::Tile> table;
    std::array<Vector2, /* A hexagon has six sides. */ 6> mirrorCenterTable;
    int radius;

  public:

    explicit HexagonMaze(int const setRadius=0);

    constexpr int getRadius() const { return radius; }

    constexpr Vector2::HashMap<Maze::Tile> const &getTable() const { return table; }

    std::size_t getTileCount() const override;

    Tile &at(Vector2 const &key) override;
    Tile const &at(Vector2 const &key) const override;

    void forEachTile(std::function<void(Vector2 const &, Tile const)> const &) const override;

    void forEachValidDirection(std::function<void(Direction const)> const &) const override;

    std::tuple<Vector2, bool> checkAdjacent(Vector2 key, Direction const direction) const override;

    Vector2 wrapKey(Vector2 const &key) const;
};

#endif
