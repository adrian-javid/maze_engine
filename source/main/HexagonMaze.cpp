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

Vector2 HexagonMaze::wrapKey(Vector2 const &key) const {
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

auto HexagonMaze::at(Vector2 const &key) -> Tile & { return table.at(HexagonMaze::wrapKey(key)); }

auto HexagonMaze::at(Vector2 const &key) const -> Tile const & { return table.at(HexagonMaze::wrapKey(key)); }

void HexagonMaze::forEachTile(std::function<void(Vector2 const &, Tile const)> const &forThisTile) const {
    for (auto const &[key, tile] : table) forThisTile(key, tile);
}

void HexagonMaze::forEachPrincipalDirection(std::function<void(Direction const)> const &forThisPrincipalDirection) const {
    forThisPrincipalDirection(northeast);
    forThisPrincipalDirection(east     );
    forThisPrincipalDirection(southeast);
}

auto HexagonMaze::checkAdjacent(Vector2 key, Direction const direction) const -> TileAdjacency {
    switch (direction) {
        case northeast:
            return {wrapKey(key + Vector2::hexagonNortheast), at(key) & northeast};
        case east     :
            return {wrapKey(key + Vector2::hexagonEast), at(key) & east};
        case southeast:
            return {wrapKey(key + Vector2::hexagonSoutheast), at(key) & southeast};

        case southwest:
            key = wrapKey(key + Vector2::hexagonSouthwest);
            return {key, table.at(key) & northeast};
        case west     :
            key = wrapKey(key + Vector2::hexagonWest);
            return {key, table.at(key) & east};
        case northwest:
            key = wrapKey(key + Vector2::hexagonNorthwest);
            return {key, table.at(key) & southeast};

        default:
            throw direction;
    }
}
