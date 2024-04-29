#ifndef simpleSearch_hpp
#define simpleSearch_hpp

#include <optional>
#include <type_traits>
#include <stack>

namespace Project {
    template <typename Storage_T>
    void simpleSearch(
        Maze const &maze,
        Vector2 const &start,
        std::function<bool(Vector2 const &)> const process=nullptr
    );
}

template <typename Storage_T>
void Project::simpleSearch(
    Maze const &maze,
    Vector2 const &start,
    std::function<bool(Vector2 const &)> const processKey
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

        if (processKey) processKey(key);
        if (key == Vector2{0, 4}) {
            std::vector<Vector2> path;
            for (auto iterator = upTree.find(key); iterator->first != start; iterator = upTree.find(iterator->second)) {
                path.push_back(iterator->first);
            }
            path.push_back(start);
            return;
        }

        maze.forEachNeighbor(key, [&maze, &upTree, &key, &storage](Vector2 const &neighbor) {
            if (upTree.find(neighbor) == upTree.end()) {
                upTree.insert({neighbor, key});
                storage.push(neighbor);
            }
        });
    }
}

#endif
