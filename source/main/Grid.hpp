#ifndef Grid_hpp
#define Grid_hpp

#include <cstdint>
#include <functional>

namespace Project { class Grid; }

class Project::Grid {

  public:

    using Tile = uint_fast8_t;
    using Index = int_fast32_t;

    constexpr static Tile emptyTile = 0;
    constexpr static Tile wallFlag = 1 << 0;

    virtual Tile &at(Index const row, Index const column) = 0;
    virtual Tile const &at(Index const row, Index const column) const = 0;

    bool isWall(Index const row, Index const column) const;
    void putWall(Index const row, Index const column);

    // virtual void forNeighbor(
    //   Index const row, Index const column,
    //   std::function<void(Index const, Index const)> const 
    // ) const;

};

#endif
