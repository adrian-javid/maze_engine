#ifndef MazeEngine_Search_GreedyBestFirst_hpp
#define MazeEngine_Search_GreedyBestFirst_hpp

#include "maze_engine/vector2.hpp"
#include "maze_engine/maze.hpp"
#include "maze_engine/search/abstract.hpp"

namespace MazeEngine {
	[[deprecated]]
	Vector2::HashMap<Vector2> greedyBestFirstSearch(
		Maze const &maze,
		Vector2 const &start, Vector2 const &end,
		std::function<bool(Vector2 const &)> const processKey=nullptr
	);

	struct GreedyBestFirstSearchHeuristicComparator {
		Maze const *maze; Vector2 end;
		explicit GreedyBestFirstSearchHeuristicComparator(Maze const &maze, Vector2 end): maze(&maze), end(std::move(end)) {}
		[[nodiscard]] bool operator()(Vector2 const &leftKey, Vector2 const &rightKey) const {
			return maze->length(leftKey - end) > maze->length(rightKey - end);
		};
	};

	using GreedyBestFirstSearchPriorityQueue = std::priority_queue<Vector2, std::vector<Vector2>, GreedyBestFirstSearchHeuristicComparator>;

	struct GreedyBestFirstSearchIterator : AbstractSearchIterator<GreedyBestFirstSearchPriorityQueue> {
		explicit GreedyBestFirstSearchIterator(Maze const &maze, Vector2 start, Vector2 end):
			AbstractSearchIterator<GreedyBestFirstSearchPriorityQueue>(
				maze,
				std::move(start),
				GreedyBestFirstSearchPriorityQueue(GreedyBestFirstSearchHeuristicComparator(maze, std::move(end)))
			)
		{}
	};
}

#endif
