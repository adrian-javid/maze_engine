#ifndef MazeEngine_Search_AStar_hpp
#define MazeEngine_Search_AStar_hpp

#include "maze_engine/vector2.hpp"
#include "maze_engine/maze.hpp"
#include "maze_engine/search/abstract.hpp"

namespace MazeEngine {
	[[deprecated]]
	Vector2::HashMap<Vector2> aStarSearch(
		Maze const &maze,
		Vector2 const &start, Vector2 const &end,
		std::function<bool(Vector2 const &)> const processKey=nullptr
	);
}

#endif
