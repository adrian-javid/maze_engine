#ifndef MazeEngine_Maze_hpp
#define MazeEngine_Maze_hpp

#include "vector2.hpp"

#include <functional>
#include <optional>
#include <cstdint>

namespace MazeEngine { struct Maze; }

struct MazeEngine::Maze {

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
		/**/  west = 1u << 6u,     none =        0u,          east = 1u << 2u,
		/**/
		/**/         southwest = 1u << 5u,       southeast = 1u << 3u,
		/**/
		/**/                       south = 1u << 4u,
	};

	static constexpr std::string_view getName(Direction const direction) {
		switch (direction) {
			using namespace std::string_view_literals;

			/**/                                       case north: return "north"sv;
			/**/
			/**/          case northwest: return "northwest"sv;              case northeast: return "northeast"sv;
			/**/
			/**/  case west: return "west"sv;           case none: return "none"sv;           case east: return "east"sv;
			/**/
			/**/          case southwest: return "southwest"sv;              case southeast: return "southeast"sv;
			/**/
			/**/                                       case south: return "south"sv;

			default: return "unknown"sv;
		}
	}

	static constexpr Direction reverseDirection(Direction const direction) {
		switch (direction) {
			/**/                                       case north: return south;
			/**/
			/**/          case northwest: return southeast;              case northeast: return southwest;
			/**/
			/**/  case west: return east;        default: assert(false); return none;        case east: return west;
			/**/
			/**/          case southwest: return northeast;              case southeast: return northwest;
			/**/
			/**/                                       case south: return north;
		}
	}

	virtual std::size_t getTileCount() const = 0;

	virtual Tile &at(Vector2 const &key) = 0;
	virtual Tile const &at(Vector2 const &key) const = 0;

	virtual void forEachKey(std::function<void(Vector2 const &)> const &) const = 0;

	virtual void forEachPrincipalDirection(std::function<void(Direction const)> const &) const = 0;

	void forEachValidDirection(std::function<void(Direction const)> const &) const;

	struct TileAdjacency {
		Vector2 key;
		bool hasWall;
		// This function is to address narrowing conversion to `bool`.
		template <typename ConvertableToBoolT>
		constexpr /* implicit on purpose */ TileAdjacency(Vector2 const &tileKey, ConvertableToBoolT const wallFlag):
			key(tileKey), hasWall{static_cast<bool>(wallFlag)}
		{ static_assert(std::is_convertible_v<ConvertableToBoolT, bool>); }
	};

	/*
		Query information about a tile that is adjacent from the direction
		with respect to the tile at the position
		indicated by the tile key.

		The object returned has the position of the adjacent tile
		and a boolean value representing whether there is a wall
		dividing the two adjacent tiles.

		If the adjacent tile would be out-of-bounds for the maze,
		the tile that is considered adjacent is from applying "wrap-around".
	*/
	virtual TileAdjacency checkAdjacent(Vector2 const tileKey, Direction const direction) const = 0;

	virtual Vector2 const &getOffset(Direction const direction) const = 0;
	virtual bool isInBounds(Vector2 const &key) const = 0;

	virtual int length(Vector2 const &key) const = 0;

	void forEachNeighbor(Vector2 const &, std::function<void(Vector2 const &)> const &) const;

	[[deprecated("Can use a maze generation iterator instead.")]]
	void generate(unsigned int const seed, bool const wrap=true);

	constexpr Maze() = default;
	constexpr Maze(Maze const &) = default;
	MazeEngine::Maze &operator=(const MazeEngine::Maze &) = default;
	virtual ~Maze() = default;

};

#endif
