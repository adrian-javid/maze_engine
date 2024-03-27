#ifndef Grid_hpp
#define Grid_hpp

#include <functional>
#include "CommonInt.hpp"
#include "Vector2.hpp"

namespace Project { class Grid; }

class Project::Grid {

  public:

    using Tile = std::uint_fast8_t;

    constexpr static Tile emptyTile = 0;
    constexpr static Tile wallFlag = 1 << 0;

    virtual Tile &at(CommonInt const row, CommonInt const column) = 0;
    virtual Tile const &at(CommonInt const row, CommonInt const column) const = 0;

    bool isWall(CommonInt const row, CommonInt const column) const;
    void putWall(CommonInt const row, CommonInt const column);

    virtual void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const = 0;

    virtual ~Grid() = default;

};

#endif
