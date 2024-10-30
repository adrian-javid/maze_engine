#include "a_star.hpp"

#include "maze_engine/search/abstract.hpp"
#include <queue>

auto MazeEngine::aStarSearch(
	Maze const &maze,
	Vector2 const &start, Vector2 const &end,
	std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
	using MetaVertex = AStarSearchIterator::MetaVertex;

	std::priority_queue<MetaVertex, std::vector<MetaVertex>, std::greater<MetaVertex>> frontier;
	frontier.push({start, 0});

	Vector2::HashMap<Vector2> upTree{{start, start}};
	Vector2::HashMap<int> costMap{{start, 0}};

	while (not frontier.empty()) {
		MetaVertex const tileKey(frontier.top());
		frontier.pop();

		if (processKey != nullptr and processKey(tileKey.vector)) break;

		maze.forEachNeighbor(tileKey.vector, [&tileKey, &costMap, &frontier, &upTree, &maze, &end](Vector2 const &neighbor) {
			int const derivedCost{costMap.at(tileKey.vector) + /* cost to move adjacent */1};

			if (costMap.find(neighbor) == costMap.end() or derivedCost < costMap.at(neighbor)) {
				costMap.insert({neighbor, derivedCost});
				int priority{derivedCost + /* heuristic */maze.length(neighbor - end)};
				frontier.push({neighbor, priority});
				upTree.insert({neighbor, tileKey.vector});
			}
		});
	}

	return upTree;
}
