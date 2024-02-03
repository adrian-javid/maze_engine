#ifndef breadth_first_search_hpp
#define breadth_first_search_hpp

#include "SquareGrid.hpp"
#include "Vector2.hpp"
#include <optional>

namespace Project {
    std::optional<std::vector<Vector2>> breadthFirstSearch(SquareGrid const &grid, Vector2 const &start, Vector2 const &end);
}

#endif
