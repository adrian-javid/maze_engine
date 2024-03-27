
#ifndef simpleSearch_hpp
#define simpleSearch_hpp

#include <optional>
#include <vector>
#include <array>
#include <type_traits>
#include <stack>
#include "Cast.hpp"

#include "Vector2.hpp"

namespace Project {
    template <typename Storage_T>
    std::optional<std::vector<Vector2>> simpleSearch(
        SquareGrid const &, Vector2 const &, Vector2 const &
    );
}

template <typename Storage_T>
std::optional<std::vector<Project::Vector2>> Project::simpleSearch(
    SquareGrid const &grid,
    Vector2 const &start,
    Vector2 const &end
) {
    using namespace Project;

    static const std::array<Vector2, 4> cardinalDirectionList = {Vector2::north, Vector2::south, Vector2::east, Vector2::west};

    int const rowCount = Cast::toInt(grid.getRowCount());
    int const columnCount = Cast::toInt(grid.getColumnCount());

    Storage_T storage;
    Vector2::HashMap<Vector2> upTree;

    storage.push(start);
    upTree.insert({start, start});

    while (not storage.empty()) {
        Vector2 const key = [&storage]() constexpr -> Vector2 const & {
            if constexpr (std::is_same_v<Storage_T, std::stack<Vector2>>)
                return storage.top();
            else
                return storage.front();
        }();
        storage.pop();

        if (key == end) {
            std::vector<Vector2> path;
            for (auto iterator = upTree.find(key); iterator->first != start; iterator = upTree.find(iterator->second)) {
                path.push_back(iterator->first);
            }
            path.push_back(start);
            return path;
        }

        grid.forNeighbor(key, [&grid, &upTree, &key, &storage](Vector2 const &neighbor) {
            bool const neighborIsWall = grid.isWall(neighbor.row, neighbor.col);

            if (not neighborIsWall && not upTree.count(neighbor)) {
                upTree.insert({neighbor, key});
                storage.push(neighbor);
            }
        });
    }

    return std::nullopt;
}

#endif
