#include "breadthFirstSearch.hpp"

#include <queue>
#include "simpleSearch.hpp"

std::optional<std::vector<Project::Vector2>> Project::breadthFirstSearch(
    Grid const &grid, Vector2 const &start, Vector2 const &end
) {
    return Project::simpleSearch<std::queue<Vector2>>(grid, start, end);
}
