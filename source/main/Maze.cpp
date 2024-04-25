
#include "Maze.hpp"

using namespace Project;

auto Maze::at(Vector2 const &key) -> Tile & { return at(key.value1, key.value2); }
auto Maze::at(Vector2 const &key) const -> Tile const & { return at(key.value1, key.value2); };

bool Maze::isWall(int const row, int const column) const { return at(row, column) & Maze::wallFlag; }
void Maze::putWall(int const row, int const column) { at(row, column) |= Maze::wallFlag; }
