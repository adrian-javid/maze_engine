
#include "Grid.hpp"

using namespace Project;

bool Grid::isWall(CommonInt const row, CommonInt const column) const { return at(row, column) & Grid::wallFlag; }
void Grid::putWall(CommonInt const row, CommonInt const column) { at(row, column) |= Grid::wallFlag; }
