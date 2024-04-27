#ifndef Maze_hpp
#define Maze_hpp

#include "Vector2.hpp"

#include <functional>
#include <optional>

namespace Project { class Maze; }

class Project::Maze {

  public:

    using Tile = std::uint_least8_t;

    static constexpr Tile emptyTile{0u};

    /**
     * @note Purposely unscoped `enum` for bitwise operations and implicit conversion to `bool`.
     */
    enum Direction : Tile {
      /**/                       north = 1u << 0u,
      /**/
      /**/         northwest = 1u << 7u,       northeast = 1u << 1u,
      /**/
      /**/  west = 1u << 6u,                                east = 1u << 2u,
      /**/
      /**/         southwest = 1u << 5u,        southeast = 1u << 3u,
      /**/
      /**/                       south = 1u << 4u,
    };

    static constexpr Direction reverseDirection(Direction const direction) {
      switch (direction) {
      /**/                                       case north: return south;
      /**/
      /**/          case northwest: return southeast;              case northeast: return southwest;
      /**/
      /**/  case west: return east;          default: throw direction;                 case east: return west;
      /**/
      /**/          case southwest: return northeast;              case southeast: return northwest;
      /**/
      /**/                                       case south: return north;
      }
    }

    virtual std::size_t getTileCount() const = 0;

    virtual Tile &at(Vector2 const &key) = 0;
    virtual Tile const &at(Vector2 const &key) const = 0;

    virtual void forEachTile(std::function<void(Vector2 const &, Tile const)> const &) const = 0;

    virtual void forEachValidDirection(std::function<void(Direction const)> const &) const = 0;

    struct TileAdjacency {
      Vector2 key;
      bool hasWall;
      // This function is to address narrowing conversion to `bool`.
      template <typename ConvertableToBool_T>
      constexpr /* implicit on purpose */ TileAdjacency(Vector2 const &tileKey, ConvertableToBool_T const wallFlag):
        key(tileKey), hasWall{static_cast<bool>(wallFlag)}
      {}
    };
    virtual TileAdjacency checkAdjacent(Vector2, Direction const) const = 0;

    void forEachNeighbor(Vector2 const &, std::function<void(Vector2 const &)> const &) const;

    void shuffle(unsigned int const seed);

    constexpr Maze() = default;
    constexpr Maze(Maze const &) = default;
    Project::Maze &operator=(const Project::Maze &) = default;
    virtual ~Maze() = default;

};

#endif
