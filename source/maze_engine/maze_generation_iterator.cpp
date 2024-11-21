#include "maze_generation_iterator.hpp"

MazeEngine::MazeGenerationIterator::MazeGenerationIterator(
	Maze &paramMaze, unsigned int const seed, bool const wrap, std::size_t const paramExcessWallPruneCountdown
):
	cyclePrevention(paramMaze.getTileCount()), maze{paramMaze}, excessWallPruneCountdown{paramExcessWallPruneCountdown}
{
	UnionFinder::Identifier indentifierCount{0};

	maze.forEachKey([this, wrap, &indentifierCount](Vector2 const key) {
		identity.insert({key, indentifierCount++});
		maze.forEachPrincipalDirection([this, wrap, key](Maze::Direction const direction) {
			if (not wrap and not maze.isInBounds(key + maze.getOffset(direction))) return;
			if (maze.checkAdjacent(key, direction).hasWall) wallList.push_back({key, direction});
		});
	});

	std::mt19937 randomNumberGenerator(seed);
	std::shuffle(wallList.begin(), wallList.end(), randomNumberGenerator);

	wallIterator = wallList.cbegin();
}

/*
	In this function, the maze generation iterator must be advanced one.

	This means that the iterator must process one more wall,
	provided there is at least one wall leftover.
*/
auto MazeEngine::MazeGenerationIterator::advance() -> Result {
	/*
		It is likely a logic error to call this function when all of
		the walls to process have been exhausted. In debug builds,
		this assertion catches that case.
	*/
	assert(not isDone());

	/*
		We pay a little runtime cost to ensure that this function
		doesn't invoke undefined behavior if the iterator is done.

		If the iterator is done, there are no walls to process.
	*/
	if (isDone()) return Result::none;

	/*
		We are using the wall iterator like a stack data structure here.
		By getting the wall and then incrementing the wall iterator,
		we are effectively popping an element from the figurative stack.
	*/
	Wall const &wall{*wallIterator};
	++wallIterator;

	UnionFinder::Identifier const
		mainTileIdentifier{identity.at(wall.tileKey)},
		neighborTileIdentifier{identity.at(maze.checkAdjacent(wall.tileKey, wall.type).key)};

	MazeEngine::Maze::Tile &tile{maze.at(wall.tileKey)};
	assert((tile & wall.type) == wall.type);

	// Check if both tiles are not already members of the same set.
	if (cyclePrevention.find(mainTileIdentifier) != cyclePrevention.find(neighborTileIdentifier)) {
		tile ^= wall.type; // flip the wall bit to zero
		cyclePrevention.unionThem(mainTileIdentifier, neighborTileIdentifier);
		return Result::didUnion;
	} else {
		if (excessWallPruneCountdown > 0) {
			--excessWallPruneCountdown;
			tile ^= wall.type; // flip the wall bit to zero
		}
		return Result::none;
	}
}
