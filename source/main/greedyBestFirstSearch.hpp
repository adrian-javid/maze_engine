#ifndef greedyBestFirstSearch_hpp
#define greedyBestFirstSearch_hpp

#include "Vector2.hpp"
#include "Maze.hpp"
#include "abstractSearch.hpp"

namespace Project {
    Vector2::HashMap<Vector2> greedyBestFirstSearch(
        Maze const &maze,
        Vector2 const &start, Vector2 const &end,
        std::function<bool(Vector2 const &)> const processKey=nullptr
    );
}

#endif
