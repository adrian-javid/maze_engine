#ifndef breadth_first_search_hpp
#define breadth_first_search_hpp

#include "SquareGrid.hpp"
#include "Vector2.hpp"
#include <optional>

namespace Project {
    /**
     * @brief Search the `grid` until starting from `start` until `end` is found.
     * Returns a path of tiles `std::vector<Vector2>` from `end` to `start` if such a path exists.
     * Returns `std::nullopt` if `end` is not found.
     * 
     * @param grid Grid will search in.
     * @param start Location of tile to start search from.
     * @param end Location of tile to search for.
     * @return `std::optional<std::vector<Vector2>>`
     */
    std::optional<std::vector<Vector2>> breadthFirstSearch(SquareGrid const &grid, Vector2 const &start, Vector2 const &end);
}

#endif
