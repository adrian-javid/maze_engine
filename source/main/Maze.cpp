
#include "Maze.hpp"

using namespace Project;

bool Maze::isWall(int const row, int const column) const { return at({row, column}) & Maze::wallFlag; }
void Maze::putWall(int const row, int const column) { at({row, column}) |= Maze::wallFlag; }
