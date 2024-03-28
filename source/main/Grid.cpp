
#include "Grid.hpp"

using namespace Project;

bool Grid::isWall(int const row, int const column) const { return at(row, column) & Grid::wallFlag; }
void Grid::putWall(int const row, int const column) { at(row, column) |= Grid::wallFlag; }
