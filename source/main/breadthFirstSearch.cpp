#include "breadthFirstSearch.hpp"

#include <queue>
#include "simpleSearch.hpp"

void Project::breadthFirstSearch(
    Maze const &maze, Vector2 const &start, std::function<bool(Vector2 const &)> const processKey
) {
    return Project::simpleSearch<std::queue<Vector2>>(maze, start, processKey);
}
