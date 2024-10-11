#include "vector2.hpp"
#include <sstream>
#include <cassert>

using MazeEngine::Vector2;

std::string Vector2::toString() const {
	std::stringstream buffer;
	buffer << '(' << value1 << ", " << value2 << ")";
	return buffer.str();
}

std::ostream& MazeEngine::operator<<(std::ostream &outputStream, Vector2 const &vector) {
	outputStream << vector.toString(); return outputStream;
}
