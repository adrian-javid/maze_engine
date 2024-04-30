#include "aStarSearch.hpp"

#include "abstractSearch.hpp"
#include <queue>

auto Project::aStarSearch(
    Maze const &maze,
    Vector2 const &start, Vector2 const &end,
    std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
    struct MetaVertex {
        Vector2 key;
        // Smaller priority is better.
        int priority;
        inline bool operator>(MetaVertex const &vertex) const {
            return priority > vertex.priority;
        }
    };

    std::priority_queue<MetaVertex, std::vector<MetaVertex>, std::greater<MetaVertex>> frontier;
    frontier.push({start, 0});

    Vector2::HashMap<Vector2> upTree{{start, start}};
    Vector2::HashMap<int> costMap{{start, 0}};

    while (not frontier.empty()) {
        MetaVertex const vertex(std::move(frontier.top()));
        frontier.pop();

        if (processKey != nullptr and processKey(vertex.key)) break;

        maze.forEachNeighbor(vertex.key, [&vertex, &costMap, &frontier, &upTree, &maze, &end](Vector2 const &neighbor) {
            int const derivedCost{costMap.at(vertex.key) + /* cost to move adjacent */1};
            auto const costIter(costMap.find(neighbor));

            if (costIter == costMap.end() or derivedCost < costIter->second) {
                costIter->second = derivedCost;
                int priority{derivedCost + /* heuristic */maze.length(neighbor - end)};
                frontier.push({vertex.key, priority});
                upTree[neighbor] = vertex.key;
            }
        });
    }

    return upTree;
}
