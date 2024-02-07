
#include "Grid.hpp"

using namespace Project;

bool Grid::isWall(size_t const row, size_t const column) const { return at(row, column) & Grid::WALL; }
void Grid::putWall(size_t const row, size_t const column) { at(row, column) |= Grid::WALL; }
