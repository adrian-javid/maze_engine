#include "breadth_first.hpp"

auto MazeEngine::breadthFirstSearch(
	Maze const &maze, Vector2 const &start, std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
	std::queue<Vector2> queue;
	return MazeEngine::abstractSearch<decltype(queue)>(maze, start, queue, processKey);
}
