#include "maze.hpp"

#include "maze_engine/union_finder.hpp"

#include <algorithm>
#include <random>

void MazeEngine::Maze::forEachValidDirection(std::function<void(Direction const)> const &forThisDirection) const {
	forEachPrincipalDirection([&forThisDirection](Direction const direction) -> void {
		forThisDirection(direction);
	});

	forEachPrincipalDirection([&forThisDirection](Direction const direction) -> void {
		forThisDirection(reverseDirection(direction));
	});
}

void MazeEngine::Maze::forEachNeighbor(Vector2 const key, std::function<void(Vector2 const)> const &forThisNeighbor) const {
	forEachValidDirection([this, key, &forThisNeighbor](Direction const direction) -> void {
		auto const &&[neighbor, wallFlag]{checkAdjacent(key, direction)};
		if (not wallFlag) forThisNeighbor(neighbor);
	});
}

auto MazeEngine::Maze::generate(unsigned int const seed, bool const wrap) -> void {
	UnionFinder::Identifier identifierCount{0};
	Vector2::HashMap<UnionFinder::Identifier> identity;

	struct Wall {
		Vector2 tileKey;
		Direction type;
	};

	std::vector<Wall> wallList;

	forEachKey([this, wrap, &wallList, &identity, &identifierCount](Vector2 const key) {
		identity.insert({key, identifierCount++});
		forEachPrincipalDirection([this, wrap, &wallList, key](Direction const direction) {
			if (not wrap and not isInBounds(key + getOffset(direction))) return;
			if (checkAdjacent(key, direction).hasWall) wallList.push_back({key, direction});
		});
	});

	std::mt19937 randomNumberGenerator(seed);
	std::shuffle(wallList.begin(), wallList.end(), randomNumberGenerator);

	UnionFinder cyclePrevention(getTileCount());

	for (Wall const &wall : wallList) {
		UnionFinder::Identifier const thisId{identity.at(wall.tileKey)};
		UnionFinder::Identifier const adjId{identity.at(checkAdjacent(wall.tileKey, wall.type).key)};

		if (cyclePrevention.find(thisId) != cyclePrevention.find(adjId)) {
			at(wall.tileKey) ^= wall.type; // flip the wall bit to zero
			cyclePrevention.unionThem(thisId, adjId);
		}
	}

	return;
}
