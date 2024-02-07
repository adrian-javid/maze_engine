#ifndef Grid_hpp
#define Grid_hpp

#include  <cstdint>

namespace Project { class Grid; }

class Project::Grid {

  public:

    using Tile = uint_fast8_t;
    using Index = int_fast32_t;

    constexpr static Tile NONE = 0;
    constexpr static Tile WALL = 1 << 0;

    virtual Tile &at(size_t const row, size_t const column) = 0;
    virtual Tile const &at(size_t const row, size_t const column) const = 0;

    bool isWall(size_t const row, size_t const column) const;
    void putWall(size_t const row, size_t const column);

};

#endif
