#include "SquareMaze.hpp"

using namespace Project;

SquareMaze::SquareMaze(): SquareMaze(0, 0, emptyTile) {}

SquareMaze::SquareMaze(int const rowCount, int const columnCount, Tile const tileFillValue):
	table([rowCount, columnCount]() constexpr -> std::size_t {
		if (rowCount < 0)
			throw std::invalid_argument("row count " + std::to_string(rowCount) + " should not be negative");

		if (columnCount < 0)
			throw std::invalid_argument("column count " + std::to_string(columnCount) + " should not be negative");

		return static_cast<std::size_t>(rowCount) * static_cast<std::size_t>(columnCount);
	}(), tileFillValue), rowCount{rowCount}, columnCount{columnCount}
{}

int SquareMaze::getRowCount() const { return rowCount; }
int SquareMaze::getColumnCount() const { return columnCount; }
auto SquareMaze::getTable() const -> std::vector<Tile> const & { return table; }

std::size_t SquareMaze::getTileCount() const { return table.size(); }

auto SquareMaze::at(Vector2 const &tileKey) -> Tile & {
	return table.at(getFlatIndex(wrapKey(tileKey)));
}

auto SquareMaze::at(Vector2 const &tileKey) const -> Tile const & {
	return table.at(getFlatIndex(wrapKey(tileKey)));
}

void SquareMaze::forEachKey(std::function<void(Vector2 const &)> const &forThisKey) const {
	for (Vector2 key(0); key.value1 < rowCount; ++key.value1)
		for (key.value2 = 0; key.value2 < columnCount; ++key.value2)
			forThisKey(key);
}

void SquareMaze::forEachPrincipalDirection(std::function<void(Direction const)> const &forThisPrincipalDirection) const {
	forThisPrincipalDirection(north);
	forThisPrincipalDirection(east);
}

auto SquareMaze::checkAdjacent(Vector2 key, Direction const direction) const -> TileAdjacency {
	switch (direction) {
		case Direction::north:
			return {wrapKey(key + northOffset), at(key) & north};
		case Direction::east :
			return {wrapKey(key + eastOffset), at(key) & east};

		case Direction::south:
			key = wrapKey(key + southOffset);
			return {key, table.at(getFlatIndex(key)) & north};
		case Direction::west :
			key = wrapKey(key + westOffset);
			return {key, table.at(getFlatIndex(key)) & east};

		default:
			throw direction;
	}
}

std::string SquareMaze::toString(char const wallSymbol, char const emptySymbol) const {
	std::stringstream buffer;

	for (Vector2 key(0); key.value1 < rowCount; ++key.value1) {
		for (key.value2 = 0; key.value2 < columnCount; ++key.value2) {
			Tile const tile = at(key);
			char const symbol = (tile ? wallSymbol : emptySymbol);
			buffer << ' ' << symbol;
		}
		buffer << ' ' << '\n';
	}

	return buffer.str();
}

std::ostream &Project::operator<<(std::ostream &outputStream, SquareMaze const &squareGrid) {
	outputStream << squareGrid.toString(); return outputStream;
}

Vector2 const &SquareMaze::getOffset(Direction const direction) const {
	switch (direction) {
		case Direction::north: return northOffset;
		case Direction::east : return eastOffset;
		case Direction::south: return southOffset;
		case Direction::west : return westOffset;

		default:
			throw direction;
	}
}

bool SquareMaze::isInBounds(Vector2 const &key) const {
	return
		0 <= key.value1 and key.value1 < rowCount and
		0 <= key.value2 and key.value2 < columnCount
	;
}

int SquareMaze::length(Vector2 const &key) const {
	return key.manhattanLength();
}
