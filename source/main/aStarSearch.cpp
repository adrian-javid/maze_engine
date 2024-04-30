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
        int priority;
        inline bool operator>(MetaVertex const &vertex) const;
    };

    std::priority_queue<MetaVertex, std::vector<MetaVertex>, std::greater<MetaVertex>> frontier;
    frontier.emplace(start, 0);

    Vector2::HashMap<Vector2> upTree{{start, start}};
    Vector2::HashMap<int> costMap{{start, 0}};

    while (not frontier.empty()) {
        MetaVertex const vertex(std::move(frontier.top()));
        frontier.pop();

        if (processKey != nullptr and processKey(vertex.key)) break;

        maze.forEachNeighbor(vertex.key, [&vertex, &costMap, &frontier, &upTree](Vector2 const &neighbor) {
            if (
                int const derivedCost{costMap.at(vertex.key) + int()},
                decltype(costMap.end()) const costIter(costMap.find(neighbor));
                costIter == costMap.end() or derivedCost < costIter->second
            ) {
                costIter->second = derivedCost;
                int priority{derivedCost + int()};
                frontier.emplace(vertex.key, priority);
                upTree[neighbor] = vertex.key;
            }
        });
    }

    return upTree;
}
