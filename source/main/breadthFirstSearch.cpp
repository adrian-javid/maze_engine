#include "breadthFirstSearch.hpp"

#include <queue>
#include "simpleSearch.hpp"

std::optional<std::vector<Project::Vector2>> Project::breadthFirstSearch(
    Maze const &grid, Vector2 const &start, Vector2 const &end, VertexProcessor const processor
) {
    return Project::simpleSearch<std::queue<Vector2>>(grid, start, end, processor);
}
