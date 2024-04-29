#ifndef simpleSearch_hpp
#define simpleSearch_hpp

#include <optional>
#include <type_traits>
#include <stack>

namespace Project {
    template <typename Storage_T>
    Vector2::HashMap<Vector2> simpleSearch(
        Maze const &maze,
        Vector2 const &start,
        std::function<bool(Vector2 const &)> const process=nullptr
    );
}

template <typename Storage_T>
auto Project::simpleSearch(
    Maze const &maze,
    Vector2 const &start,
    std::function<bool(Vector2 const &)> const processKey
) -> Vector2::HashMap<Vector2> {
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
