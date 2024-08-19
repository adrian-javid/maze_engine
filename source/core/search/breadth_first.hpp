#ifndef core_search_BREADTH_FIRST_HPP
#define core_search_BREADTH_FIRST_HPP

#include "core/maze.hpp"
#include "core/vector2.hpp"
#include "core/search/abstract.hpp"
#include <optional>

namespace Project {
	/**
	 * @brief Search the `grid` until starting from `start` until `end` is found.
	 * Returns a path of tiles `std::vector<Vector2>` from `end` to `start` if such a path exists.
	 * Returns `std::nullopt` if `end` is not found.
	 * 
	 * @param grid Maze will search in.
	 * @param start Location of tile to start search from.
	 */
	Vector2::HashMap<Vector2> breadthFirstSearch(
		Maze const &maze, Vector2 const &start, std::function<bool(Vector2 const &)> const processKey=nullptr
	);

	struct BreadthFirstSearchIterator : AbstractSearchIterator<std::queue<Vector2>> {
		explicit BreadthFirstSearchIterator(Maze const &maze, Vector2 start):
			AbstractSearchIterator<std::queue<Vector2>>(maze, std::move(start), std::queue<Vector2>())
		{}
	};
}

#endif
