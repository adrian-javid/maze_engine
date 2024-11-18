#include "vector2.hpp"
#include <sstream>
#include <cassert>

std::ostream& MazeEngine::operator<<(std::ostream &outputStream, Vector2 const vector) {
	outputStream << '(' << vector.value1 << ", " << vector.value2 << ')';
	return outputStream;
}
