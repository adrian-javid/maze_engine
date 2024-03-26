#include "simple_search.hpp"
#include "cast.hpp"
#include <queue>
#include <map>
#include <array>

std::optional<std::vector<Project::Vector2>> Project::breadthFirstSearch(SquareGrid const &grid, Vector2 const &start, Vector2 const &end) {
    using namespace Project;

    static std::array<Vector2, 4> const cardinalDirectionList = {Vector2::north, Vector2::south, Vector2::east, Vector2::west};

    int const rowCount = Cast::toInt(grid.getRowCount());
    int const columnCount = Cast::toInt(grid.getColumnCount());

    std::queue<Vector2> queue;
    Vector2::HashMap<Vector2> map;

    queue.push(start);
    map.insert({start, start});

    while (not queue.empty()) {
        Vector2 const vector = queue.front();
        queue.pop();

        if (vector == end) {
            std::vector<Vector2> path;
            for (auto iterator = map.find(vector); iterator->first != start; iterator = map.find(iterator->second)) {
                path.push_back(iterator->first);
            }
            path.push_back(start);
            return path;
        }

        for (Vector2 const &cardinalDirection : cardinalDirectionList) {
            Vector2 const neighbor = (vector + cardinalDirection).wrap(rowCount, columnCount);

            bool const neighborIsWall = grid.isWall(neighbor.row, neighbor.col);

            if (not neighborIsWall && not map.count(neighbor)) {
                map.insert({neighbor, vector});
                queue.push(neighbor);
            }
        }
    }

    return std::nullopt;
}
