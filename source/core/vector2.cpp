#include "vector2.hpp"
#include <sstream>
#include <cassert>

using MazeEngine::Vector2;

std::size_t Vector2::Hash::operator()(Vector2 const &vector) const noexcept {
	static_assert(std::is_same_v<decltype(vector.value1), decltype(vector.value2)>);
	static constexpr std::hash<decltype(vector.value1)> hash;
	size_t const hashRow = hash(vector.value1);
	size_t const hashCol = hash(vector.value2);
	/*
		This is a copy of `boost::hash_combine`.
		https://www.boost.org/doc/libs/1_55_0/doc/html/hash/reference.html#boost.hash_combine
	*/
	return hashRow ^ (hashCol + 0x9e3779b9 + (hashRow << 6) + (hashRow >> 2));
}

std::string Vector2::toString() const {
	std::stringstream buffer;
	buffer << '(' << value1 << ", " << value2 << ")";
	return buffer.str();
}

std::ostream& MazeEngine::operator<<(std::ostream &outputStream, Vector2 const &vector) {
	outputStream << vector.toString(); return outputStream;
}
