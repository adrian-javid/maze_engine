#include "breadthFirstSearch.hpp"

#include <queue>
#include "simpleSearch.hpp"

std::optional<std::vector<Project::Vector2>> Project::breadthFirstSearch(
    SquareGrid const &grid, Vector2 const &start, Vector2 const &end
) {
    return Project::simpleSearch<std::queue>(grid, start, end);
}
