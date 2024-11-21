#include "maze_engine/vector2.hpp"
#include "maze_engine/maze/square.hpp"


namespace App::CompileTimeTests {

	using namespace MazeEngine;

	namespace Neighbors {
		static constexpr int rowCount{20};
		static constexpr int columnCount{20};

		static constexpr Vector2 vector(1, 1);
		static_assert(vector == Vector2(1, 1));

		static constexpr Vector2 northNeighbor((vector + SquareMaze::northOffset).wrap(rowCount, columnCount));
		static_assert(northNeighbor == Vector2(0, 1));

		static constexpr Vector2 southNeighbor((vector + SquareMaze::southOffset).wrap(rowCount, columnCount));
		static_assert(southNeighbor == Vector2(2, 1));

		static constexpr Vector2 eastNeighbor((vector + SquareMaze::eastOffset).wrap(rowCount, columnCount));
		static_assert(eastNeighbor == Vector2(1, 2));

		static constexpr Vector2 westNeighbor((vector + SquareMaze::westOffset).wrap(rowCount, columnCount));
		static_assert(westNeighbor == Vector2(1, 0));
	}
}
