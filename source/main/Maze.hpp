#ifndef Maze_hpp
#define Maze_hpp

#include <functional>
#include "Vector2.hpp"

namespace Project { class Maze; }

class Project::Maze {

  public:
    using Tile = std::uint_least8_t;

    constexpr static Tile emptyTile{0u};

    constexpr static Tile wallFlag{0b1u};

    constexpr static Tile northWall     { 1u << 0u };
    constexpr static Tile northeastWall { 1u << 1u };
    constexpr static Tile eastWall      { 1u << 2u };
    constexpr static Tile southeastWall { 1u << 3u };

    virtual Tile &at(int const index1, int const index2) = 0;
    virtual Tile const &at(int const index1, int const index2) const = 0;

    virtual Tile &at(Vector2 const &key);
    virtual Tile const &at(Vector2 const &key) const;

    bool isWall(int const index1, int const index2) const;
    void putWall(int const index1, int const index2);

    virtual void forNeighbor(Vector2 const &, std::function<void(Vector2 const &)>) const = 0;

    enum struct Direction : std::uint_fast8_t {
      /**/                 north,
      /**/
      /**/      northwest,        northeast,
      /**/
      /**/  west,                           east,
      /**/
      /**/      southwest,        southeast,
      /**/
      /**/                 south
    };

    virtual bool isOpen(int const index1, int const index2, Direction const) const = 0;

    constexpr Maze() = default;
    constexpr Maze(Maze const &) = default;
    Project::Maze &operator=(const Project::Maze &) = default;
    virtual ~Maze() = default;

};

#endif
