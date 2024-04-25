
#ifndef simpleSearch_hpp
#define simpleSearch_hpp

#include <type_traits>
#include <stack>

namespace Project {
    template <typename Storage_T>
    std::optional<std::vector<Vector2>> simpleSearch(
        Maze const &, Vector2 const &, Vector2 const &
    );
}

template <typename Storage_T>
std::optional<std::vector<Project::Vector2>> Project::simpleSearch(
    Maze const &grid,
    Vector2 const &start,
    Vector2 const &end
) {
    using namespace Project;

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
            bool const neighborIsWall = grid.isWall(neighbor.value1, neighbor.value2);

            if (not neighborIsWall && upTree.find(neighbor) == upTree.end()) {
                upTree.insert({neighbor, key});
                storage.push(neighbor);
            }
        });
    }

    return std::nullopt;
}

#endif
