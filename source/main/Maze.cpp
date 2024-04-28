
#include "Maze.hpp"
#include "UnionFinder.hpp"

#include <algorithm>
#include <random>

void Project::Maze::forEachValidDirection(std::function<void(Direction const)> const &forThisDirection) const {
    forEachPrincipalDirection([&forThisDirection](Direction const direction) -> void {
        forThisDirection(direction);
    });

    forEachPrincipalDirection([&forThisDirection](Direction const direction) -> void {
        forThisDirection(reverseDirection(direction));
    });
}

void Project::Maze::forEachNeighbor(Vector2 const &key, std::function<void(Vector2 const &)> const &forThisNeighbor) const {
    forEachValidDirection([this, &key, &forThisNeighbor](Direction const direction) {
        auto const &&[neighbor, wallFlag] = checkAdjacent(key, direction);
        if (not wallFlag) forThisNeighbor(neighbor);
    });
}

#if true
#include <iostream>
static auto &o = std::cout;
static char const ln = '\n';
#endif

auto Project::Maze::shuffle(unsigned int const seed) -> void {
    UnionFinder::Identifier indentifierCount{0};
    Vector2::HashMap<UnionFinder::Identifier> identity;

    struct Wall {
        Vector2 tileKey;
        Direction type;
    };

    std::vector<Wall> wallList;

    forEachKey([this, &wallList, &identity, &indentifierCount](Vector2 const &key) {
        identity.insert({key, indentifierCount++});
        forEachPrincipalDirection([this, &wallList, &key](Direction const direction) {
            if (checkAdjacent(key, direction).hasWall) wallList.push_back({key, direction});
        });
    });

    std::mt19937 randomNumberGenerator(seed);
    std::shuffle(wallList.begin(), wallList.end(), randomNumberGenerator);

    UnionFinder cyclePrevention(getTileCount());

    for (Wall const &wall : wallList) {
        o << wall.tileKey << ln;

        UnionFinder::Identifier const thisId{identity.at(wall.tileKey)};
        UnionFinder::Identifier const adjId{identity.at(checkAdjacent(wall.tileKey, wall.type).key)};

        if (cyclePrevention.find(thisId) != cyclePrevention.find(adjId)) {
            at(wall.tileKey) ^= wall.type; // flip the wall bit to zero
            cyclePrevention.unionThem(thisId, adjId);
        }
    }

    return;
}
