#ifndef Grid_hpp
#define Grid_hpp

#include "CommonInt.hpp"

#include <functional>

namespace Project { class Grid; }

class Project::Grid {

  public:

    using Tile = uint_fast8_t;

    constexpr static Tile emptyTile = 0;
    constexpr static Tile wallFlag = 1 << 0;

    virtual Tile &at(CommonInt const row, CommonInt const column) = 0;
    virtual Tile const &at(CommonInt const row, CommonInt const column) const = 0;

    bool isWall(CommonInt const row, CommonInt const column) const;
    void putWall(CommonInt const row, CommonInt const column);

    // virtual void forNeighbor(
    //   CommonInt const row, CommonInt const column,
    //   std::function<void(CommonInt const, CommonInt const)> const 
    // ) const;

};

#endif
