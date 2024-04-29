#include "breadthFirstSearch.hpp"

#include <queue>
#include "simpleSearch.hpp"

std::optional<std::vector<Project::Vector2>> Project::breadthFirstSearch(
    Maze const &maze, Vector2 const &start, Vector2 const &end, std::function<void(Vector2 const &)> const processor
) {
    return Project::simpleSearch<std::queue<Vector2>>(maze, start, end, processor);
}
