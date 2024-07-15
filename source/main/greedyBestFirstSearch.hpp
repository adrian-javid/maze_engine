#ifndef greedyBestFirstSearch_hpp
#define greedyBestFirstSearch_hpp

#include "Vector2.hpp"
#include "Maze.hpp"
#include "abstractSearch.hpp"

namespace Project {
    Vector2::HashMap<Vector2> greedyBestFirstSearch(
        Maze const &maze,
        Vector2 const &start, Vector2 const &end,
        std::function<bool(Vector2 const &)> const processKey=nullptr
    );

    struct GreedyBestFirstSearchHeuristicComparator {
        Maze const *maze; Vector2 end;
        explicit GreedyBestFirstSearchHeuristicComparator(Maze const &maze, Vector2 end): maze(&maze), end(std::move(end)) {}
        [[nodiscard]] bool operator()(Vector2 const &leftKey, Vector2 const &rightKey) const {
            return maze->length(leftKey - end) > maze->length(rightKey - end);
        };
    };

    using GreedyBestFirstSearchPriorityQueue = std::priority_queue<Vector2, std::vector<Vector2>, GreedyBestFirstSearchHeuristicComparator>;

    struct GreedyBestFirstSearchIterator : AbstractSearchIterator<GreedyBestFirstSearchPriorityQueue> {
        explicit GreedyBestFirstSearchIterator(Maze const &maze, Vector2 start, Vector2 end):
            AbstractSearchIterator<GreedyBestFirstSearchPriorityQueue>(
                maze,
                std::move(start),
                GreedyBestFirstSearchPriorityQueue(GreedyBestFirstSearchHeuristicComparator(maze, std::move(end)))
            )
        {}
    };
}

#endif
