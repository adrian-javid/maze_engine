#ifndef MazeEngine_Vector2_hpp
#define MazeEngine_Vector2_hpp

#include <unordered_set>
#include <unordered_map>
#include <array>
#include <cstdint>
#include <ostream>

#include "auxiliary.hpp"

namespace MazeEngine { struct Vector2; }

/**
 * @brief Ordered pair of `std::int_least32_t`. Can represent the identifier of a tile in a grid.
 */
struct MazeEngine::Vector2 {
	using Value = std::int_least32_t;
	Value value1{0};
	Value value2{0};

	constexpr Value thirdAxis() const { return -value1 - value2; }

	/*
		Returns `true` if the vector's values are integers that are either `-1`, `0`, or `+1`,
		otherwise returns `false`.
	*/
	[[nodiscard]] constexpr
	bool isSimple() const {
		switch (value1) {
			case -1:
			case  0:
			case +1: switch (value2) {
				case -1:
				case  0:
				case +1: return true;
			}
		}
		return false;
	}

	template<Value index, Value upperBound>
	static constexpr std::size_t rotateIndex(Value const indexOffsetter) {
		static_assert(index >= 0);
		static_assert(index < upperBound);
		return static_cast<std::size_t>(Aux::wrap(index - indexOffsetter, upperBound));
	}

	constexpr Vector2 hexagonalRotate(Value const indexDegree60) const {
		std::array<Value, 3> vector{};
		Value const signFactor{indexDegree60 % 2 == 0 ? 1 : -1};
		vector[rotateIndex<0, vector.size()>(indexDegree60)] = this->value1;
		vector[rotateIndex<1, vector.size()>(indexDegree60)] = this->value2;
		vector[rotateIndex<2, vector.size()>(indexDegree60)] = this->thirdAxis();
		return Vector2(vector[0], vector[1]) * signFactor;
	}

	/**
	 * @brief Create a vector with zero-initialized values.
	 */
	constexpr Vector2(): value1{0}, value2{0} {}

	/**
	 * @brief Create a vector with specified values.
	 * @param paramValue1 assign `value1`
	 * @param paramValue2 assign `value2`
	*/
	constexpr Vector2(Value paramValue1, Value paramValue2): value1{paramValue1}, value2{paramValue2} {}

	constexpr Value manhattanLength() const {
		return Aux::abs(value1) + Aux::abs(value2);
	}

	constexpr Value hexManhattanLength() const {
		return (manhattanLength() + Aux::abs(thirdAxis())) / 2;
	}

	constexpr Vector2 operator+(Vector2 const vector) const {
		return {value1 + vector.value1, value2 + vector.value2};
	}

	constexpr Vector2 &operator+=(Vector2 const vector) {
		return (*this = *this + vector);
	}

	constexpr Vector2 operator-(Vector2 const vector) const {
		return {value1 - vector.value1, value2 - vector.value2};
	}

	constexpr Vector2 &operator-=(Vector2 const vector) {
		return (*this = *this - vector);
	}

	constexpr bool operator==(Vector2 const vector) const {
		return value1 == vector.value1 and value2 == vector.value2;
	}

	constexpr bool operator!=(Vector2 const vector) const { return not(*this == vector); }

	constexpr bool operator<(Vector2 const vector) const {
		if (value1 != vector.value1)
			return value1 < vector.value1;
		else
			return value2 < vector.value2;
	}

	constexpr bool operator>=(Vector2 const vector) const { return not(*this < vector); }

	constexpr bool operator>(Vector2 const vector) const { return vector < *this; }

	constexpr bool operator<=(Vector2 const vector) const { return not(vector < *this); }

	constexpr Vector2 operator*(Value const scalar) const {
		return Vector2(this->value1 * scalar, this->value2 * scalar);
	}

	constexpr Vector2 wrap(Value const rowCount, Value const columnCount) const {
		return Vector2(Aux::wrap(value1, rowCount), Aux::wrap(value2, columnCount));
	}

	struct [[nodiscard]] Hash {[[nodiscard]] std::size_t operator()(Vector2 const vector) const noexcept {
		static_assert(
			std::is_same_v<decltype(vector.value1), Value> and
			std::is_same_v<decltype(vector.value2), Value>
		);
		return Aux::combineHashValues(
			std::hash<Value>{}(vector.value1),
			std::hash<Value>{}(vector.value2)
		);
	}};

	/// @brief An unordered set of `Vector2`.
	using HashSet = std::unordered_set<Vector2, Hash>;

	/**
	 * @brief An unordered hash map from `Vector2` to `T`.
	 * @tparam T value mapped from `Vector2`
	 */
	template<typename T> using HashMap = std::unordered_map<Vector2, T, Hash>;
};

namespace MazeEngine {
	std::ostream& operator<<(std::ostream &outputStream, Vector2 const vector);
}

#endif
