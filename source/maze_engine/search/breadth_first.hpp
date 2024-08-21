#ifndef MazeEngine_Search_BreadthFirst_hpp
#define MazeEngine_Search_BreadthFirst_hpp

#include "maze_engine/maze.hpp"
#include "maze_engine/vector2.hpp"
#include "maze_engine/search/abstract.hpp"
#include <optional>

namespace MazeEngine {
	/**
	 * @brief Search the `grid` until starting from `start` until `end` is found.
	 * Returns a path of tiles `std::vector<Vector2>` from `end` to `start` if such a path exists.
	 * Returns `std::nullopt` if `end` is not found.
	 * 
	 * @param grid Maze will search in.
	 * @param start Location of tile to start search from.
	 */
	[[deprecated]]
	Vector2::HashMap<Vector2> breadthFirstSearch(
		Maze const &maze, Vector2 const &start, std::function<bool(Vector2 const &)> const processKey=nullptr
	);

	struct BreadthFirstSearchIterator : AbstractSearchIterator<std::queue<Vector2>> {
		explicit BreadthFirstSearchIterator(Maze const &mazeReference, Vector2 start):
			AbstractSearchIterator<std::queue<Vector2>>(mazeReference, std::move(start), std::queue<Vector2>())
		{}
	};
}

#endif
