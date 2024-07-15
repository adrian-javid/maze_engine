#include "greedyBestFirstSearch.hpp"
#include "abstractSearch.hpp"

auto Project::greedyBestFirstSearch(
    Maze const &maze,
    Vector2 const &start, Vector2 const &end,
    std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
    /*
        The comparator returns whether `(left, right)` are in sorted order, least to greatest.

        In other words,
        "Is `(heuristic(left), heuristic(right))` sorted from "worst" value to "best" value?"

        The heuristic is the distance to end of the maze, so, `(left, right)` are sorted if
        `distance(left) > distance(right)`, because a higher distance is "worse".
    */
    auto const heuristicComparator = [&maze, &end](Vector2 const &leftKey, Vector2 const &rightKey) -> bool {
        return maze.length(leftKey - end) > maze.length(rightKey - end);
    };

    std::priority_queue<Vector2, std::vector<Vector2>, decltype(heuristicComparator)> priorityQueue(heuristicComparator);

    return abstractSearch<decltype(priorityQueue)>(maze, start, priorityQueue, processKey);
}
