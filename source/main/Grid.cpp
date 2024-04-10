
#include "Grid.hpp"

using namespace Project;

auto Grid::at(Vector2 const &key) -> Tile & { return at(key.value1, key.value2); }
auto Grid::at(Vector2 const &key) const -> Tile const & { return at(key.value1, key.value2); };

bool Grid::isWall(int const row, int const column) const { return at(row, column) & Grid::wallFlag; }
void Grid::putWall(int const row, int const column) { at(row, column) |= Grid::wallFlag; }
