#ifndef MazeEngine_Maze_Hexagon_hpp
#define MazeEngine_Maze_Hexagon_hpp

#include "maze_engine/maze.hpp"

#include <array>

namespace MazeEngine { class HexagonMaze; }

class MazeEngine::HexagonMaze : public MazeEngine::Maze {

	private:

		Vector2::HashMap<Maze::Tile> table;
		// This does not include the center tile.
		Vector2::Value outerRadius;
		std::array<Vector2, std::size_t{/* A hexagon has six sides. */6u}> mirrorCenterTable;

	public:

		static Vector2 constexpr northwestOffset{ 0, -1};
		static Vector2 constexpr northeastOffset{+1, -1};
		static Vector2 constexpr      eastOffset{+1,  0};
		static Vector2 constexpr southeastOffset{ 0, +1};
		static Vector2 constexpr southwestOffset{-1, +1};
		static Vector2 constexpr      westOffset{-1,  0};

		static constexpr std::array<Vector2, 6u> offsetList{
			/* 0 */ northeastOffset,
			/* 1 */      eastOffset,
			/* 2 */ southeastOffset,
			/* 3 */ southwestOffset,
			/* 4 */      westOffset,
			/* 5 */ northwestOffset,
		};

		static constexpr std::array<std::array<Direction, 3u>, 3u> directionMatrix{
			/*    */                          /* -1 */         /*  0 */              /* +1 */
			/* -1 */ std::array{ Direction::     none, Direction::west, Direction::southwest, },
			/*  0 */ std::array{ Direction::northwest, Direction::none, Direction::southeast, },
			/* +1 */ std::array{ Direction::northeast, Direction::east, Direction::     none, },
		};

		static_assert(
			Maze::getSimpleDirection<HexagonMaze>(northwestOffset) == Direction::northwest and
			Maze::getSimpleDirection<HexagonMaze>(northeastOffset) == Direction::northeast and
			Maze::getSimpleDirection<HexagonMaze>(     eastOffset) == Direction::     east and
			Maze::getSimpleDirection<HexagonMaze>(southeastOffset) == Direction::southeast and
			Maze::getSimpleDirection<HexagonMaze>(southwestOffset) == Direction::southwest and
			Maze::getSimpleDirection<HexagonMaze>(     westOffset) == Direction::     west
		);

		static constexpr char const *errorMessageForInvalidDirection{"Invalid direction for hexagon maze."};

		[[nodiscard]]
		explicit HexagonMaze(Vector2::Value const radiusValue=0, Tile const tileFillValue=emptyTile);

		/*
			This does not include the center tile.
		*/
		[[nodiscard]]
		constexpr Vector2::Value getOuterRadius() const { return outerRadius; }

		[[nodiscard]]
		constexpr Vector2::HashMap<Maze::Tile> const & getTable() const { return table; }

		[[nodiscard]]
		std::size_t getTileCount() const override;

		[[nodiscard]]
		Tile &at(Vector2 const key) override;

		[[nodiscard]]
		Tile const &at(Vector2 const key) const override;

		template <typename CallableT>
		constexpr std::enable_if_t<std::is_invocable_v<CallableT, Vector2 const>, void> forEachKeyInRing(
			Vector2 const center, Vector2::Value const ringIndex, CallableT &&(forThisKeyInRing)
		) const {
			assert(ringIndex >= 0);

			if (ringIndex == 0u) return static_cast<void>(std::forward<CallableT>(forThisKeyInRing)(center));

			for (Vector2 const entry : offsetList) {
				Vector2 const key(entry * ringIndex);
				Vector2 const flank(entry.hexagonalRotate(2));

				for (Vector2::Value index{0}; index < ringIndex; ++index) {
					forThisKeyInRing(key + flank * index);
				}
			}
		}

		void forEachKey(std::function<void(Vector2 const)> const &) const override;

		void forEachPrincipalDirection(std::function<void(Direction const)> const &) const override;

		[[nodiscard]]
		TileAdjacency checkAdjacent(Vector2 const key, Direction const direction) const override;

		[[nodiscard]]
		Vector2 getOffset(Direction const direction) const override;

		[[nodiscard]]
		static constexpr bool isSimpleOffsetVector(Vector2 const vector) {
			return
				vector == northwestOffset or
				vector == northeastOffset or
				vector ==      eastOffset or
				vector == southeastOffset or
				vector == southwestOffset or
				vector ==      westOffset;
		}

		[[nodiscard]]
		bool isInBounds(Vector2 const key) const override;

		[[nodiscard]]
		int length(Vector2 const key) const override;

		[[nodiscard]]
		Vector2 wrapKey(Vector2 const key) const;
};

#endif
