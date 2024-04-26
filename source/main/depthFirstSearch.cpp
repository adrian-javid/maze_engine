#include "depthFirstSearch.hpp"

#include <stack>
#include "simpleSearch.hpp"

std::optional<std::vector<Project::Vector2>> Project::depthFirstSearch(
    Maze const &grid, Vector2 const &start, Vector2 const &end, Maze::VertexAction const processor
) {
    return Project::simpleSearch<std::stack<Vector2>>(grid, start, end, processor);
}
