#include "depthFirstSearch.hpp"

#include <stack>
#include "simpleSearch.hpp"

std::optional<std::vector<Project::Vector2>> Project::depthFirstSearch(
    Grid const &grid, Vector2 const &start, Vector2 const &end
) {
    return Project::simpleSearch<std::stack<Vector2>>(grid, start, end);
}
