#include "hexagon.hpp"

using namespace MazeEngine;

static Vector2 calculateMirrorCenter(int const index, int const radius) {
	static constexpr Vector2 center(0, 0);
	Vector2 const startPosition{2 * radius + 1, -radius};
	Vector2 const difference(startPosition - center);
	Vector2 const rotatedDifference(difference.hexagonalRotate(index));
	Vector2 const rotatedPosition(rotatedDifference + center);
	return rotatedPosition;
}

Vector2 HexagonMaze::wrapKey(Vector2 const &key) const {
	// return early if key is in table
	auto iterator(table.find(key));
	if (iterator != table.end())
		return key;

	/* key is out of bounds */

	struct ClosestMirrorCenterCandidate {
		Vector2 mirrorCenter;
		int distance;
	} closestMirrorCenter{mirrorCenterTable[0], (key - mirrorCenterTable[0]).hexManhattanLength()};

	// find the closest mirror center
	for (std::uint_fast8_t mirrorIndex{0u + 1u}; mirrorIndex < 6u; ++mirrorIndex) {
		int const distance{(key - mirrorCenterTable[mirrorIndex]).hexManhattanLength()};
		if (distance < closestMirrorCenter.distance)
		closestMirrorCenter = {mirrorCenterTable[mirrorIndex], distance};
	}

	// subtract mirror center from key until the key is in-bounds
	Vector2 safeKey(key - closestMirrorCenter.mirrorCenter); 
	for (
		iterator = table.find(safeKey);
		iterator == table.end();
		iterator = table.find(safeKey -= closestMirrorCenter.mirrorCenter)
	) {
		continue;
	}

	return safeKey;
}

HexagonMaze::HexagonMaze(int const setRadius, Tile const tileFillValue):
	table(),
	mirrorCenterTable{
		calculateMirrorCenter(0, setRadius),
		calculateMirrorCenter(1, setRadius),
		calculateMirrorCenter(2, setRadius),
		calculateMirrorCenter(3, setRadius),
		calculateMirrorCenter(4, setRadius),
		calculateMirrorCenter(5, setRadius),
	},
	radius{setRadius}
{
	assert(radius >= 0);

	static constexpr Vector2 center(0, 0);

	for (int q{-radius}; q <= radius; ++q) {
		int r1{std::max<int>(-radius, -q - radius)};
		int r2{std::min<int>( radius, -q + radius)};

		for (int r{r1}; r <= r2; ++r) {
			Vector2 const key(center + Vector2(q, r));
			table.insert({key, tileFillValue});
		}
	}
}

std::size_t HexagonMaze::getTileCount() const { return table.size(); }

auto HexagonMaze::at(Vector2 const &key) -> Tile & { return table.at(HexagonMaze::wrapKey(key)); }

auto HexagonMaze::at(Vector2 const &key) const -> Tile const & { return table.at(HexagonMaze::wrapKey(key)); }

void HexagonMaze::forEachKey(std::function<void(Vector2 const &)> const &forThisKey) const {
	for (auto const &pair : table) forThisKey(pair.first);
}

void HexagonMaze::forEachPrincipalDirection(std::function<void(Direction const)> const &forThisPrincipalDirection) const {
	forThisPrincipalDirection(northeast);
	forThisPrincipalDirection(east     );
	forThisPrincipalDirection(southeast);
}

auto HexagonMaze::checkAdjacent(Vector2 key, Direction const direction) const -> TileAdjacency {
	switch (direction) {
		case northeast:
			return {wrapKey(key + northeastOffset), at(key) & northeast};
		case east     :
			return {wrapKey(key + eastOffset), at(key) & east};
		case southeast:
			return {wrapKey(key + southeastOffset), at(key) & southeast};

		case southwest:
			key = wrapKey(key + southwestOffset);
			return {key, table.at(key) & northeast};
		case west     :
			key = wrapKey(key + westOffset);
			return {key, table.at(key) & east};
		case northwest:
			key = wrapKey(key + northwestOffset);
			return {key, table.at(key) & southeast};

		default:
			throw direction;
	}
}

Vector2 const &HexagonMaze::getOffset(Direction const direction) const {
	switch (direction) {
		case northeast: return northeastOffset;
		case east     : return eastOffset;
		case southeast: return southeastOffset;
		case southwest: return southwestOffset;
		case west     : return westOffset;
		case northwest: return northwestOffset;

		default:
			throw direction;
	}
}

bool HexagonMaze::isInBounds(Vector2 const &key) const {
	return table.find(key) != table.end();
}

int HexagonMaze::length(Vector2 const &key) const {
	return key.hexManhattanLength();
}
