
#include "Maze.hpp"
#include "UnionFinder.hpp"

#include <algorithm>
#include <random>

auto Project::Maze::generate(unsigned int const seed) -> void {
    struct Wall {
        Vector2 tilKey;
        int identifier;
        Maze::Direction type;
    };

    std::vector<Wall> wallList;

    std::mt19937 randomNumberGenerator(seed);
    std::shuffle(wallList.begin(), wallList.end(), randomNumberGenerator);

    UnionFinder cyclePrevention(getTileCount());
    while (not wallList.empty()) {
        Wall const &wall = wallList.back();
        wallList.pop_back();
    }
}
