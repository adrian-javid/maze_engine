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

	static constexpr Direction reverseDirection(Direction const direction) {
		switch (direction) {
		/**/                                       case north: return south;
		/**/
		/**/          case northwest: return southeast;              case northeast: return southwest;
		/**/
		/**/  case west: return east;              default: throw direction;             case east: return west;
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
		template <typename ConvertableToBool_T>
		constexpr /* implicit on purpose */ TileAdjacency(Vector2 const &tileKey, ConvertableToBool_T const wallFlag):
			key(tileKey), hasWall{static_cast<bool>(wallFlag)}
		{ static_assert(std::is_convertible_v<ConvertableToBool_T, bool>); }
	};
	virtual TileAdjacency checkAdjacent(Vector2, Direction const) const = 0;

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
