#ifndef MazeEngine_Maze_Square_hpp
#define MazeEngine_Maze_Square_hpp

// parent class
#include "maze_engine/maze.hpp"

// grid to string
#include <string>
#include <sstream>

// table of tiles
#include <vector>

#include "maze_engine/vector2.hpp"
#include <array>

namespace MazeEngine {
	class SquareMaze;
	std::ostream &operator<<(std::ostream &outputStream, SquareMaze const &squareGrid);
}

class MazeEngine::SquareMaze : public MazeEngine::Maze {

	public:

		static constexpr Vector2 northOffset{-1,  0};
		static constexpr Vector2 southOffset{+1,  0};
		static constexpr Vector2  eastOffset{ 0, +1};
		static constexpr Vector2  westOffset{ 0, -1};

		static constexpr std::array<std::array<Direction, 3u>, 3u> directionMatrix{
			/*    */                      /* -1 */         /*  0 */          /* +1 */
			/* -1 */ std::array{ Direction:: none, Direction::north, Direction:: none, },
			/*  0 */ std::array{ Direction:: west, Direction:: none, Direction:: east, },
			/* +1 */ std::array{ Direction:: none, Direction::south, Direction:: none, },
		};

		static_assert(
			Maze::getSimpleDirection<SquareMaze>(northOffset) == Direction::north and
			Maze::getSimpleDirection<SquareMaze>(southOffset) == Direction::south and
			Maze::getSimpleDirection<SquareMaze>( eastOffset) == Direction::east  and
			Maze::getSimpleDirection<SquareMaze>( westOffset) == Direction::west
		);

		static constexpr char const *errorMessageForInvalidDirection{"Invalid direction for square maze."};

	private:

		std::vector<Tile> table;
		static_assert(not std::is_same_v<std::vector<Tile>, std::vector<bool>>);
		int rowCount;
		int columnCount;

	public:

		[[nodiscard]]
		explicit SquareMaze();
		
		[[nodiscard]]
		explicit SquareMaze(int const rowCount, int const columnCount, Tile const tileFillValue=emptyTile);
		
		[[nodiscard]]
		int getRowCount() const;
		
		[[nodiscard]]
		int getColumnCount() const;

		[[nodiscard]]
		std::vector<Tile> const &getTable() const;

		[[nodiscard]]
		std::size_t getTileCount() const override;

		[[nodiscard]]
		Tile &at(Vector2 const) override;

		[[nodiscard]]
		Tile const &at(Vector2 const) const override;

		void forEachKey(std::function<void(Vector2 const)> const &) const override;

		void forEachPrincipalDirection(std::function<void(Direction const)> const &) const override;

		[[nodiscard]]
		TileAdjacency checkAdjacent(Vector2, Direction const) const override;

		[[nodiscard]]
		constexpr Vector2 wrapKey(Vector2 const key) const { return key.wrap(rowCount, columnCount); }

		[[nodiscard]]
		constexpr std::size_t getFlatIndex(Vector2 const key) const {
			int const flatIndex{/* row */key.value1 * columnCount + /* column */key.value2};
			return static_cast<std::size_t>(flatIndex);
		}

		[[nodiscard]]
		std::string toString(char const wallSymbol='#', char const emptySymbol='.') const;

		[[nodiscard]]
		Vector2 getOffset(Direction const direction) const override;

		[[nodiscard]]
		bool isInBounds(Vector2 const key) const override;

		[[nodiscard]]
		int length(Vector2 const key) const override;
};

#endif
