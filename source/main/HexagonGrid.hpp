#ifndef HexagonGrid_hpp
#define HexagonGrid_hpp

#include "Grid.hpp"

#include <array>

namespace Project { class HexagonGrid; }

class Project::HexagonGrid : public Project::Grid {

  private:

    Vector2::HashMap<Grid::Tile> table;
    std::array<Vector2, /* A hexagon has six sides. */ 6> mirrorCenterTable;
    int radius;

  public:

    explicit HexagonGrid(int const setRadius=0);

    constexpr int Radius() const { return radius; }

    constexpr Vector2::HashMap<Grid::Tile> const &core() const { return table; }

    Tile &at(int const axis1, int const axis2) override;
    Tile const &at(int const axis1, int const axis2) const override;

    virtual Tile &at(Vector2 const &key) override;
    virtual Tile const &at(Vector2 const &key) const override;

    void forNeighbor(Vector2 const &key, std::function<void(Vector2 const &)> operate) const override;

    Vector2 wrap(Vector2 const &key) const;
};

#endif
