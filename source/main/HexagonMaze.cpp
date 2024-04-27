#include "HexagonMaze.hpp"

using namespace Project;

static Vector2 calculateMirrorCenter(int const index, int const radius) {
    constexpr Vector2 center(0, 0);
    Vector2 const startPosition{2 * radius + 1, -radius};
    Vector2 const difference = startPosition - center;
    Vector2 const rotatedDifference = difference.hexagonalRotate(index);
    Vector2 const rotatedPosition = rotatedDifference + center;
    return rotatedPosition;
}

Vector2 HexagonMaze::wrap(Vector2 const &key) const {
    // return early if key is in table
    auto iterator = table.find(key);
    if (iterator != table.end())
        return key;

    /* key is out of bounds */

    struct ClosestMirrorCenterCandidate {
        Vector2 mirrorCenter;
        int distance;
    } closestMirrorCenter{mirrorCenterTable[0], (key - mirrorCenterTable[0]).hexManhattanLength()};

    // find the closest mirror center
    for (std::uint_fast8_t mirrorIndex = 0u + 1u; mirrorIndex < 6u; ++mirrorIndex) {
        auto const distance = (key - mirrorCenterTable[mirrorIndex]).hexManhattanLength();
        if (distance < closestMirrorCenter.distance)
        closestMirrorCenter = {mirrorCenterTable[mirrorIndex], distance};
    }

    // subtract mirror center from key until the key is in-bounds
    auto safeKey = key - closestMirrorCenter.mirrorCenter; 
    for (
        iterator = table.find(safeKey);
        iterator == table.end();
        iterator = table.find(safeKey -= closestMirrorCenter.mirrorCenter)
    ) {
        continue;
    }

    return safeKey;
}

HexagonMaze::HexagonMaze(int const setRadius):
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

    constexpr Vector2 center(0, 0);

    for (int q = -radius; q <= radius; ++q) {
        int r1 = std::max<int>(-radius, -q - radius);
        int r2 = std::min<int>( radius, -q + radius);

        for (int r = r1; r <= r2; ++r) {
            Vector2 const key(center + Vector2(q, r));
            table.insert({key, Project::Maze::emptyTile});
        }
    }
}

std::size_t HexagonMaze::getTileCount() const { return table.size(); }

auto HexagonMaze::at(Vector2 const &key) -> Tile & { return table.at(HexagonMaze::wrap(key)); }

auto HexagonMaze::at(Vector2 const &key) const -> Tile const & { return table.at(HexagonMaze::wrap(key)); }

void HexagonMaze::forEachTile(std::function<void(Vector2 const &, Tile const)> const &forThisTile) const {
    for (auto const &[key, tile] : table) forThisTile(key, tile);
}

void HexagonMaze::forEachValidDirection(std::function<void(Direction)> const &forThisDirection) const {
    forThisDirection(Direction::northeast);
    forThisDirection(Direction::east     );
    forThisDirection(Direction::southeast);
    forThisDirection(Direction::southwest);
    forThisDirection(Direction::west     );
    forThisDirection(Direction::northwest);
}

void HexagonMaze::forNeighbor(Vector2 const &key, std::function<void(Vector2 const &)> const &operate) const {
    if (not hasWall(key, Direction::northwest)) operate(HexagonMaze::wrap(key + Vector2::hexagonNorthwest));
    if (not hasWall(key, Direction::northeast)) operate(HexagonMaze::wrap(key + Vector2::hexagonNortheast));
    if (not hasWall(key, Direction::east     )) operate(HexagonMaze::wrap(key + Vector2::hexagonEast     ));
    if (not hasWall(key, Direction::southeast)) operate(HexagonMaze::wrap(key + Vector2::hexagonSoutheast));
    if (not hasWall(key, Direction::southwest)) operate(HexagonMaze::wrap(key + Vector2::hexagonSouthwest));
    if (not hasWall(key, Direction::west     )) operate(HexagonMaze::wrap(key + Vector2::hexagonWest     ));
}

bool HexagonMaze::hasWall(Vector2 const &tileKey, Direction const direction) const {
    switch (direction) {
        case Direction::northeast: return at(tileKey                            ) & HexagonMaze::northeastWall;
        case Direction::east     : return at(tileKey                            ) & HexagonMaze::eastWall     ;
        case Direction::southeast: return at(tileKey                            ) & HexagonMaze::southeastWall;
        case Direction::southwest: return at(tileKey + Vector2::hexagonSouthwest) & HexagonMaze::northeastWall;
        case Direction::west     : return at(tileKey + Vector2::hexagonWest     ) & HexagonMaze::eastWall     ;
        case Direction::northwest: return at(tileKey + Vector2::hexagonNorthwest) & HexagonMaze::southeastWall; 

        default:
            return false;
    }
}
