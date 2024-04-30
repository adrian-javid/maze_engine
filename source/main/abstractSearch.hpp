#ifndef simpleSearch_hpp
#define simpleSearch_hpp

#include <optional>
#include <type_traits>
#include <stack>
#include <queue>

#include "Vector2.hpp"

namespace Project {
    template <typename Storage_T>
    Vector2::HashMap<Vector2> abstractSearch(
        Maze const &maze,
        Vector2 const &start,
        Storage_T &storage,
        std::function<bool(Vector2 const &)> const processKey
    );
}

template <typename Storage_T>
auto Project::abstractSearch(
    Maze const &maze,
    Vector2 const &start,
    Storage_T &storage,
    std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
    using namespace Project;

    Vector2::HashMap<Vector2> upTree;

    storage.push(start);
    upTree.insert({start, start});

    while (not storage.empty()) {
        Vector2 const key = [&storage]() constexpr -> Vector2 const & {
            if constexpr (std::is_same_v<Storage_T, std::stack<Vector2>>)
                return storage.top();
            else if constexpr (std::is_same_v<Storage_T, std::queue<Vector2>>)
                return storage.front();
            else /* other, such as priority queue */
                return storage.top();
        }();
        storage.pop();

        if (processKey) if (processKey(key)) break;

        maze.forEachNeighbor(key, [&maze, &upTree, &key, &storage](Vector2 const &neighbor) {
            if (upTree.find(neighbor) == upTree.end()) {
                upTree.insert({neighbor, key});
                storage.push(neighbor);
            }
        });
    }

    return upTree;
}

#endif
