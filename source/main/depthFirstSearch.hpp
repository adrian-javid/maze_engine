#ifndef depthFirstSearch_hpp
#define depthFirstSearch_hpp

#include "Maze.hpp"
#include "Vector2.hpp"
#include "simpleSearch.hpp"

#include <optional>

namespace Project {
    /**
     * @brief Search the `grid` until starting from `start` until `end` is found.
     * Returns a path of tiles `std::vector<Vector2>` from `end` to `start` if such a path exists.
     * Returns `std::nullopt` if `end` is not found.
     * 
     * @param grid Maze will search in.
     * @param start Location of tile to start search from.
     * @param end Location of tile to search for.
     * @return `std::optional<std::vector<Vector2>>`
     */
    std::optional<std::vector<Vector2>> depthFirstSearch(
        Maze const &grid, Vector2 const &start, Vector2 const &end, Maze::VertexAction const processor=nullptr
    );
}

#endif
