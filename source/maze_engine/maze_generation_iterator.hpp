#ifndef MazeEngine_MazeGenerationIterator_hpp
#define MazeEngine_MazeGenerationIterator_hpp

#include "maze.hpp"
#include "union_finder.hpp"

#include <algorithm>
#include <random>
#include <cassert>

namespace MazeEngine { class MazeGenerationIterator; }

class MazeEngine::MazeGenerationIterator final {
	private:
		struct Wall { Vector2 tileKey; Maze::Direction type; };

	public:
		[[nodiscard]] explicit MazeGenerationIterator(Maze &paramMaze, unsigned int const seed, bool const wrap):
			cyclePrevention(paramMaze.getTileCount()), maze{paramMaze}
		{
			UnionFinder::Identifier indentifierCount{0};

			maze.forEachKey([this, wrap, &indentifierCount](Vector2 const &key) {
				identity.insert({key, indentifierCount++});
				maze.forEachPrincipalDirection([this, wrap, &key](Maze::Direction const direction) {
					if (not wrap and not maze.isInBounds(key + maze.getOffset(direction))) return;
					if (maze.checkAdjacent(key, direction).hasWall) wallList.push_back({key, direction});
				});
			});

			std::mt19937 randomNumberGenerator(seed);
			std::shuffle(wallList.begin(), wallList.end(), randomNumberGenerator);

			wallIterator = wallList.cbegin();
		}

		inline void advance() {
			assert(not isDone());
			if (isDone()) return;

			Wall const &wall(*wallIterator);

			UnionFinder::Identifier const thisId{identity.at(wall.tileKey)};
			UnionFinder::Identifier const adjId{identity.at(maze.checkAdjacent(wall.tileKey, wall.type).key)};

			if (cyclePrevention.find(thisId) != cyclePrevention.find(adjId)) {
				maze.at(wall.tileKey) ^= wall.type; // flip the wall bit to zero
				cyclePrevention.unionThem(thisId, adjId);
			}
		}

		[[nodiscard]]
		inline bool isDone() { return wallIterator == wallList.cend(); }

	private:
		Vector2::HashMap<UnionFinder::Identifier> identity;
		std::vector<Wall> wallList;
		UnionFinder cyclePrevention;
		Maze &maze;
		decltype(MazeGenerationIterator::wallList.cbegin()) wallIterator;
};

#endif
