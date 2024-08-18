#ifndef aStarSearch_hpp
#define aStarSearch_hpp

#include "Vector2.hpp"
#include "Maze.hpp"

namespace Project {
	Vector2::HashMap<Vector2> aStarSearch(
		Maze const &maze,
		Vector2 const &start, Vector2 const &end,
		std::function<bool(Vector2 const &)> const processKey=nullptr
	);
}

#endif
