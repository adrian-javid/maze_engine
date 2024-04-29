#include "depthFirstSearch.hpp"

#include <stack>
#include "simpleSearch.hpp"

std::optional<std::vector<Project::Vector2>> Project::depthFirstSearch(
    Maze const &maze, Vector2 const &start, Vector2 const &end, std::function<void(Vector2 const &)> const processor
) {
    return Project::simpleSearch<std::stack<Vector2>>(maze, start, end, processor);
}
