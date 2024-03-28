#ifndef Grid_hpp
#define Grid_hpp

#include <functional>
#include "Vector2.hpp"

namespace Project { class Grid; }

class Project::Grid {

  public:

    using Tile = std::uint_fast8_t;

    constexpr static Tile emptyTile = 0;
    constexpr static Tile wallFlag = 1 << 0;

    virtual Tile &at(int const row, int const column) = 0;
    virtual Tile const &at(int const row, int const column) const = 0;

    bool isWall(int const row, int const column) const;
    void putWall(int const row, int const column);

    virtual void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const = 0;

    constexpr Grid() = default;
    constexpr Grid(Grid const &) = default;
    Project::Grid &operator=(const Project::Grid &) = default;
    virtual ~Grid() = default;

};

#endif
