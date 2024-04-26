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

    constexpr static Tile emptyTile{0u};

    constexpr static Tile northWall     { 1u << 0u };
    constexpr static Tile northeastWall { 1u << 1u };
    constexpr static Tile eastWall      { 1u << 2u };
    constexpr static Tile southeastWall { 1u << 3u };

    virtual std::size_t getTileCount() const = 0;

    virtual Tile &at(Vector2 const &key) = 0;
    virtual Tile const &at(Vector2 const &key) const = 0;

    // virtual void forValidDirection(std::function<void(Direction)> const) const = 0;

    // virtual std::optional<Tile> query(Vector2 const &, Direction const) const = 0;

    virtual bool hasWall(Vector2 const &tileKey, Direction const direction) const = 0;

    using VertexAction = std::function<void(Vector2 const &)>;
    virtual void forNeighbor(Vector2 const &key, VertexAction const operate) const = 0;

    void generate(unsigned int const seed);

    constexpr Maze() = default;
    constexpr Maze(Maze const &) = default;
    Project::Maze &operator=(const Project::Maze &) = default;
    virtual ~Maze() = default;

    class Iterator {
      public:
        Vector2 const &operator*() const;
        Iterator &operator++();
        bool operator==(Iterator const &iterator);
        bool operator!=(Iterator const &iterator);
    };

    // virtual Iterator begin() const = 0;

    // virtual Iterator end() const = 0;

};

#endif
