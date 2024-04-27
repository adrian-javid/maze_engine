
#include "Maze.hpp"
#include "UnionFinder.hpp"

#include <algorithm>
#include <random>

auto Project::Maze::shuffle(unsigned int const seed) -> void {
    int indentifierCount{0};
    struct Wall {
        Vector2 tileKey;
        int identifier;
        Maze::Direction type;
    };

    std::vector<Wall> wallList;
    forEachTile([&wallList, &indentifierCount](Vector2 const &key, Tile const tile) {
        // forEachValidDirection()
        // wallList.push_back({key, indentifierCount++, Maze::Direction::west});
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
        // ...
    }

}
