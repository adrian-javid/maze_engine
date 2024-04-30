#include "depthFirstSearch.hpp"

auto Project::depthFirstSearch(
    Maze const &maze, Vector2 const &start, std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
    std::stack<Vector2> stack;
    return Project::abstractSearch<decltype(stack)>(maze, start, stack, processKey);
}
