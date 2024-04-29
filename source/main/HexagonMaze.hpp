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

    static Vector2 constexpr northwestOffset{ 0, -1};
    static Vector2 constexpr northeastOffset{+1, -1};
    static Vector2 constexpr eastOffset     {+1,  0};
    static Vector2 constexpr southeastOffset{ 0, +1};
    static Vector2 constexpr southwestOffset{-1, +1};
    static Vector2 constexpr westOffset     {-1,  0};

    explicit HexagonMaze(int const radiusValue=0, Tile const tileFillValue=emptyTile);

    constexpr int getRadius() const { return radius; }

    constexpr Vector2::HashMap<Maze::Tile> const &getTable() const { return table; }

    std::size_t getTileCount() const override;

    Tile &at(Vector2 const &key) override;
    Tile const &at(Vector2 const &key) const override;

    void forEachKey(std::function<void(Vector2 const &)> const &) const override;

    void forEachPrincipalDirection(std::function<void(Direction const)> const &) const override;

    TileAdjacency checkAdjacent(Vector2 key, Direction const direction) const override;

    bool isInBounds(Vector2 const &key) const override;

    Vector2 wrapKey(Vector2 const &key) const;
};

#endif
