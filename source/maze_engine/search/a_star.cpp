#include "a_star.hpp"

#include "maze_engine/search/abstract.hpp"
#include <queue>

auto MazeEngine::aStarSearch(
	Maze const &maze,
	Vector2 const &start, Vector2 const &end,
	std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
	struct MetaVertex {
		Vector2 key;
		// Smaller priority is better.
		int priority;
		inline bool operator>(MetaVertex const &vertex) const {
			return priority > vertex.priority;
		}
	};

	std::priority_queue<MetaVertex, std::vector<MetaVertex>, std::greater<MetaVertex>> frontier;
	frontier.push({start, 0});

	Vector2::HashMap<Vector2> upTree{{start, start}};
	Vector2::HashMap<int> costMap{{start, 0}};

	while (not frontier.empty()) {
		MetaVertex const vertex(std::move(frontier.top()));
		frontier.pop();

		if (processKey != nullptr and processKey(vertex.key)) break;

		maze.forEachNeighbor(vertex.key, [&vertex, &costMap, &frontier, &upTree, &maze, &end](Vector2 const &neighbor) {
			int const derivedCost{costMap.at(vertex.key) + /* cost to move adjacent */1};

			if (costMap.find(neighbor) == costMap.end() or derivedCost < costMap.at(neighbor)) {
				costMap.insert({neighbor, derivedCost});
				int priority{derivedCost + /* heuristic */maze.length(neighbor - end)};
				frontier.push({neighbor, priority});
				upTree.insert({neighbor, vertex.key});
			}
		});
	}

	return upTree;
}
