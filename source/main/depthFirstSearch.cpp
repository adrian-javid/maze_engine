#include "depthFirstSearch.hpp"

#include <stack>
#include "simpleSearch.hpp"

void Project::depthFirstSearch(
    Maze const &maze, Vector2 const &start, std::function<bool(Vector2 const &)> const processKey
) {
    Project::simpleSearch<std::stack<Vector2>>(maze, start, processKey);
}
