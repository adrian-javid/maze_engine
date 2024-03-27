
#ifndef simpleSearch_hpp
#define simpleSearch_hpp

#include <optional>
#include <vector>

#include "Vector2.hpp"

namespace Project {
    template <typename Storage_T>
    std::optional<std::vector<Project::Vector2>> simpleSearch(
        SquareGrid const &grid,
        Vector2 const &start,
        Vector2 const &end
    );
}

template <typename Storage_T>
std::optional<std::vector<Project::Vector2>> Project::simpleSearch(
    SquareGrid const &grid,
    Vector2 const &start,
    Vector2 const &end
) {
    using namespace Project;

    static std::array<Vector2, 4> const cardinalDirectionList = {Vector2::north, Vector2::south, Vector2::east, Vector2::west};

    int const rowCount = Cast::toInt(grid.getRowCount());
    int const columnCount = Cast::toInt(grid.getColumnCount());

    Storage_T<Vector2> storage;
    Vector2::HashMap<Vector2> map;

    storage.push(start);
    map.insert({start, start});

    while (not storage.empty()) {
        Vector2 const vector = storage.front();
        storage.pop();

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
                storage.push(neighbor);
            }
        }
    }

    return std::nullopt;
}

#endif
