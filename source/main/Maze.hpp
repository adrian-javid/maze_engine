#ifndef Maze_hpp
#define Maze_hpp

#include "Vector2.hpp"

#include <functional>
#include <optional>

namespace Project { class Maze; }

class Project::Maze {

  public:
    using Tile = std::uint_least8_t;

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

    static constexpr Direction reverse(Direction const direction) {
      using D = Direction;
      switch (direction) {
      /**/                                       case D::north: return D::south;
      /**/
      /**/          case D::northwest: return D::southeast;              case D::northeast: return D::southwest;
      /**/
      /**/  case D::west: D::east;                  default: throw direction;                         case D::east: D::west;
      /**/
      /**/          case D::southwest: return D::northeast;              case D::southeast: return D::northwest;
      /**/
      /**/                                       case D::south: return D::north;
      }
    }

    constexpr static Tile emptyTile{0u};

    constexpr static Tile northWall     { 1u << 0u };
    constexpr static Tile northeastWall { 1u << 1u };
    constexpr static Tile eastWall      { 1u << 2u };
    constexpr static Tile southeastWall { 1u << 3u };

    virtual std::size_t getTileCount() const = 0;

    virtual Tile &at(Vector2 const &key) = 0;
    virtual Tile const &at(Vector2 const &key) const = 0;

    virtual void forEachTile(std::function<void(Vector2 const &, Tile const)> const &) const = 0;

    virtual void forEachValidDirection(std::function<void(Direction const)> const &) const = 0;

    struct TileAdjacency {
      Vector2 key;
      bool hasWall;
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
