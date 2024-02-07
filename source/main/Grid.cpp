
#include "Grid.hpp"

using namespace Project;

bool Grid::isWall(Index const row, Index const column) const { return at(row, column) & Grid::WALL; }
void Grid::putWall(Index const row, Index const column) { at(row, column) |= Grid::WALL; }
