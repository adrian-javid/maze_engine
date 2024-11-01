#include "maze_generation_iterator.hpp"

MazeEngine::MazeGenerationIterator::MazeGenerationIterator(Maze &paramMaze, unsigned int const seed, bool const wrap):
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

auto MazeEngine::MazeGenerationIterator::advance() -> Result {
	assert(not isDone());
	if (isDone()) return Result::none;

	Wall const &wall{*wallIterator};
	++wallIterator;

	UnionFinder::Identifier const thisId{identity.at(wall.tileKey)};
	UnionFinder::Identifier const adjId{identity.at(maze.checkAdjacent(wall.tileKey, wall.type).key)};

	// Check if both tiles are not already members of the same set.
	if (cyclePrevention.find(thisId) != cyclePrevention.find(adjId)) {
		maze.at(wall.tileKey) ^= wall.type; // flip the wall bit to zero
		cyclePrevention.unionThem(thisId, adjId);
		return Result::didUnion;
	} else {
		return Result::none;
	}
}
