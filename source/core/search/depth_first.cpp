#include "depth_first.hpp"

auto MazeEngine::depthFirstSearch(
	Maze const &maze, Vector2 const &start, std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
	std::stack<Vector2> stack;
	return MazeEngine::abstractSearch<decltype(stack)>(maze, start, stack, processKey);
}
