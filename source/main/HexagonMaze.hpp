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

    constexpr int Radius() const { return radius; }

    constexpr Vector2::HashMap<Maze::Tile> const &core() const { return table; }

    Tile &at(int const axis1, int const axis2) override;
    Tile const &at(int const axis1, int const axis2) const override;

    virtual Tile &at(Vector2 const &key) override;
    virtual Tile const &at(Vector2 const &key) const override;

    void forNeighbor(Vector2 const &key, std::function<void(Vector2 const &)> operate) const override;

    Vector2 wrap(Vector2 const &key) const;
};

#endif
