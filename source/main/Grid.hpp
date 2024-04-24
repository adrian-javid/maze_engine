#ifndef Grid_hpp
#define Grid_hpp

#include <functional>
#include "Vector2.hpp"

namespace Project { class Grid; }

class Project::Grid {

  public:
    using Tile = std::uint_least8_t;

    constexpr static Tile emptyTile = 0b000u;
    constexpr static Tile wallFlag = 1u << 0u;

    virtual Tile &at(int const index1, int const index2) = 0;
    virtual Tile const &at(int const index1, int const index2) const = 0;

    virtual Tile &at(Vector2 const &key);
    virtual Tile const &at(Vector2 const &key) const;

    bool isWall(int const index1, int const index2) const;
    void putWall(int const index1, int const index2);

    virtual void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const = 0;

    constexpr Grid() = default;
    constexpr Grid(Grid const &) = default;
    Project::Grid &operator=(const Project::Grid &) = default;
    virtual ~Grid() = default;

};

#endif
