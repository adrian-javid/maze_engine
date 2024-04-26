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

    constexpr Vector2::HashMap<Maze::Tile> const &core() const { return table; }

    std::size_t getTileCount() const;

    virtual Tile &at(Vector2 const &key) override;
    virtual Tile const &at(Vector2 const &key) const override;

    void forNeighbor(Vector2 const &key, VertexAction const operate) const override;

    bool hasWall(Vector2 const &tileKey, Direction const) const override;

    Vector2 wrap(Vector2 const &key) const;
};

#endif
