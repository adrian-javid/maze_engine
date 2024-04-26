
#include "Maze.hpp"
#include "UnionFinder.hpp"

#include <algorithm>
#include <random>

auto Project::Maze::generate(unsigned int const seed) -> void {
    struct Wall {
        Vector2 tileKey;
        int identifier;
        Maze::Direction type;
    };

    int indentifierCount{0};
    std::vector<Wall> wallList;

    for (Vector2 const &tileKey : *this) {
        
    }

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


auto Project::Maze::Iterator::operator*() const -> Vector2 const & {
    return {};
}

auto Project::Maze::Iterator::operator++() -> Iterator & {
    return *this;
}

bool Project::Maze::Iterator::operator==(Iterator const &iterator) {
    return {};
}

bool Project::Maze::Iterator::operator!=(Iterator const &iterator) {
    return {};
}

