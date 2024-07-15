#ifndef abstractSearch_hpp
#define abstractSearch_hpp

#include <optional>
#include <type_traits>
#include <stack>
#include <queue>

#include "Vector2.hpp"

namespace Project {
    template <typename StorageT>
    Vector2::HashMap<Vector2> abstractSearch(
        Maze const &maze,
        Vector2 const &start,
        StorageT &storage,
        std::function<bool(Vector2 const &)> const processKey
    );

    template <typename StorageT>
}

template <typename StorageT>
auto Project::abstractSearch(
    Maze const &maze,
    Vector2 const &start,
    StorageT &storage,
    std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
    using namespace Project;

    Vector2::HashMap<Vector2> history;

    storage.push(start);
    history.insert({start, start});

    while (not storage.empty()) {
        Vector2 const key = [&storage]() constexpr -> Vector2 const & {
            if constexpr (std::is_same_v<StorageT, std::stack<Vector2>>)
                return storage.top();
            else if constexpr (std::is_same_v<StorageT, std::queue<Vector2>>)
                return storage.front();
            else /* other, such as priority queue */
                return storage.top();
        }();
        storage.pop();

        if (processKey != nullptr and processKey(key)) break;

        maze.forEachNeighbor(key, [&maze, &history, &key, &storage](Vector2 const &neighbor) {
            if (history.find(neighbor) == history.end()) {
                history.insert({neighbor, key});
                storage.push(neighbor);
            }
        });
    }

    return history;
}


#endif
