#ifndef core_search_A_STAR_HPP
#define core_search_A_STAR_HPP

#include "core/vector2.hpp"
#include "core/maze.hpp"

namespace MazeEngine {
	Vector2::HashMap<Vector2> aStarSearch(
		Maze const &maze,
		Vector2 const &start, Vector2 const &end,
		std::function<bool(Vector2 const &)> const processKey=nullptr
	);
}

#endif
