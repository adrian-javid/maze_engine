#include "breadth_first_search.hpp"
#include <queue>
#include "convenience.hpp"

using namespace csm4880;

std::optional<std::vector<Vector2>> csm4880::breadthFirstSearch(SquareGrid const &grid, Vector2 const &start, Vector2 const &end) {
    static Vector2 const directionList[] = {NORTH, SOUTH, EAST, WEST};

    std::queue<Vector2> queue;
    std::unordered_map<Vector2, Vector2, Vector2::Hash> map;

    queue.push(start);
    map.insert({start, start});

    while (not queue.empty()) {
        Vector2 const &vector = queue.front();
        queue.pop();

        if (vector == end) {
            std::vector<Vector2> path;
            for (auto iterator = map.find(vector); iterator->first != start; iterator = map.find(iterator->second)) {
                path.push_back(iterator->first);
            }
            path.push_back(start);
            return path;
        }

        for (int index{0}; index < 4; ++index) {
            Vector2 const neighbor = (vector + directionList[index]).wrap(
                safeInt(grid.getRowCount()), safeInt(grid.getColumnCount())
            );

            bool const neighborIsWall = grid.isWall(neighbor.row, neighbor.col);

            if (not neighborIsWall && not map.count(neighbor)) {
                map.insert({neighbor, vector});
                queue.push(neighbor);
            }
        }
    }

    return std::nullopt;
}
