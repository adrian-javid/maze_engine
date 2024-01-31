#include "breadth_first_search.hpp"
#include "cast.hpp"
#include <queue>
#include <map>


std::optional<std::vector<csm4880::Vector2>> csm4880::breadthFirstSearch(SquareGrid const &grid, Vector2 const &start, Vector2 const &end) {
    using namespace csm4880;

    static Vector2 const directionList[] = {Vector2_NORTH, Vector2_SOUTH, Vector2_EAST, Vector2_WEST};

    int const rowCount = cast::toInt(grid.getRowCount());
    int const columnCount = cast::toInt(grid.getColumnCount());

    std::queue<Vector2> queue;
    Vector2::HashMap<Vector2> map;

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
            Vector2 const neighbor = (vector + directionList[index]).wrap(rowCount, columnCount);

            bool const neighborIsWall = grid.isWall(neighbor.row, neighbor.col);

            if (not neighborIsWall && not map.count(neighbor)) {
                map.insert({neighbor, vector});
                queue.push(neighbor);
            }
        }
    }

    return std::nullopt;
}
