#ifndef MazeEngine_Search_AStar_hpp
#define MazeEngine_Search_AStar_hpp

#include "maze_engine/vector2.hpp"
#include "maze_engine/maze.hpp"
#include "maze_engine/search/abstract.hpp"

namespace MazeEngine {
	[[deprecated]]
	Vector2::HashMap<Vector2> aStarSearch(
		Maze const &maze,
		Vector2 const start, Vector2 const end,
		std::function<bool(Vector2 const)> const processKey=nullptr
	);

	class AStarSearchIterator : public MazeSearchIterator {
		public:
			struct MetaVertex {
				Vector2 vector{};
				// Smaller priority is better.
				int priority{};
				inline bool operator>(MetaVertex const vertex) const {
					return priority > vertex.priority;
				}
			};

			/*
				The way this constructor is implemented,
				the first vertex will be processed twice.

				That's okay for the purposes of this program,
				even though that is not how I prefer for it to work.
			*/
			explicit AStarSearchIterator(Maze const &paramMaze, Vector2 const start, Vector2 const paramEnd):
				maze{&paramMaze}, currentTileKey(start), endTileKey(paramEnd),
				upTree{{start, start}}, costMap{{start, 0}}
			{
				frontier.push(MetaVertex{start, 0});
			}

			inline void advance() override {
				assert(not frontier.empty());
				if (frontier.empty()) return;

				currentTileKey = frontier.top().vector;
				frontier.pop();

				maze->forEachNeighbor(currentTileKey, [this](Vector2 const neighbor) {
					int const derivedCost{costMap.at(currentTileKey) + /* cost to move adjacent */1};

					if (costMap.find(neighbor) == costMap.end() or derivedCost < costMap.at(neighbor)) {
						costMap.insert({neighbor, derivedCost});
						int const priority{derivedCost + /* heuristic */maze->length(neighbor - endTileKey)};
						frontier.push({neighbor, priority});
						upTree.insert({neighbor, currentTileKey});
					}
				});
			}

			[[nodiscard]] Vector2 getVector() const override { return currentTileKey; }
			[[nodiscard]] bool isDone() const override { return frontier.empty(); }
			[[nodiscard]] Vector2::HashMap<Vector2> const &getHistory() const override { return upTree; }

		private:
			Maze const *maze{};
			Vector2 currentTileKey{}, endTileKey{};
			std::priority_queue<MetaVertex, std::vector<MetaVertex>, std::greater<MetaVertex>> frontier;
			// (History.)
			Vector2::HashMap<Vector2> upTree;
			Vector2::HashMap<int> costMap;
	};

}

#endif
