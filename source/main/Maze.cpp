
#include "Maze.hpp"
#include "UnionFinder.hpp"

#include <algorithm>
#include <random>

void Project::Maze::forEachValidDirection(std::function<void(Direction const)> const &forThisValidDirection) const {
    
}

void Project::Maze::forEachNeighbor(Vector2 const &key, std::function<void(Vector2 const &)> const &forThisNeighbor) const {
    forEachValidDirection([this, &key, &forThisNeighbor](Direction const direction) {
        auto const &&[neighbor, wallFlag] = checkAdjacent(key, direction);
        if (not wallFlag) forThisNeighbor(neighbor);
    });
}

auto Project::Maze::shuffle(unsigned int const seed) -> void {
    UnionFinder::Identifier indentifierCount{0};
    Vector2::HashMap<UnionFinder::Identifier> idendity;

    struct Wall {
        Vector2 tileKey;
        Direction type;
    };

    std::vector<Wall> wallList;
    forEachTile([this, &wallList, &indentifierCount](Vector2 const &key, Tile const tile) {
        forEachValidDirection([](Direction const direction) {

        });
    });

    /*
    for key in maze:
        for wallType in maze[key]:
            wallList.push(Wall{key, identifierCount++, wallType})
    */

    std::mt19937 randomNumberGenerator(seed);
    std::shuffle(wallList.begin(), wallList.end(), randomNumberGenerator);
    UnionFinder cyclePrevention(getTileCount());

    for (Wall const &wall : wallList) {
    }

}
