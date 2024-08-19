#ifndef core_VECTOR2_HPP
#define core_VECTOR2_HPP

#include "app/Util.hpp"

#include <functional>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <array>

namespace Project { struct Vector2; }

/**
 * @brief Ordered pair of `int`. Can represent the identifier of a tile in a grid.
 */
struct Project::Vector2 {
	int value1;
	int value2;

	constexpr int thirdAxis() const { return -value1 - value2; }

	template<int index, int upperBound>
	static constexpr std::size_t rotateIndex(int const indexOffsetter) {
		static_assert(index >= 0);
		static_assert(index < upperBound);
		return static_cast<std::size_t>(App::Util::wrapValue(index - indexOffsetter, upperBound));
	}

	constexpr Vector2 hexagonalRotate(int const indexDegree60) const {
		std::array<int, 3> vector{};
		int const signFactor = indexDegree60 % 2 == 0 ? 1 : -1;
		vector[rotateIndex<0, vector.size()>(indexDegree60)] = this->value1;
		vector[rotateIndex<1, vector.size()>(indexDegree60)] = this->value2;
		vector[rotateIndex<2, vector.size()>(indexDegree60)] = this->thirdAxis();
		return Vector2(vector[0], vector[1]) * signFactor;
	}

	/**
	 * @brief Create a vector with specified values.
	 * @param row assign `row`
	 * @param column assign `col`
	*/
	constexpr Vector2(int row=0, int column=0): value1{row}, value2{column} {}

	constexpr int manhattanLength() const {
		return App::Util::abs(value1) + App::Util::abs(value2);
	}

	constexpr int hexManhattanLength() const {
		return (manhattanLength() + App::Util::abs(thirdAxis())) / 2;
	}

	constexpr Vector2 operator+(Vector2 const &vector) const {
		return {value1 + vector.value1, value2 + vector.value2};
	}

	constexpr Vector2 &operator+=(Vector2 const &vector) {
		return (*this = *this + vector);
	}

	constexpr Vector2 operator-(Vector2 const &vector) const {
		return {value1 - vector.value1, value2 - vector.value2};
	}

	constexpr Vector2 &operator-=(Vector2 const &vector) {
		return (*this = *this - vector);
	}

	constexpr bool operator==(Vector2 const &vector) const { return value1 == vector.value1 && value2 == vector.value2; }

	constexpr bool operator!=(Vector2 const &vector) const { return not(*this == vector); }

	constexpr bool operator<(Vector2 const &vector) const {
		if (value1 != vector.value1)
			return value1 < vector.value1;
		else
			return value2 < vector.value2;
	}

	constexpr bool operator>=(Vector2 const &vector) const { return not(*this < vector); }

	constexpr bool operator>(Vector2 const &vector) const { return vector < *this; }

	constexpr bool operator<=(Vector2 const &vector) const { return not(vector < *this); }

	constexpr Vector2 operator*(int const scalar) const {
		return Vector2(this->value1 * scalar, this->value2 * scalar);
	}

	constexpr Vector2 wrap(int const rowCount, int const columnCount) const {
		return Vector2(App::Util::wrapValue(value1, rowCount), App::Util::wrapValue(value2, columnCount));
	}

	struct Hash { std::size_t operator()(Vector2 const &vector) const noexcept; };

	/// @brief An unorderd set of `Vector2`.
	using HashSet = std::unordered_set<Vector2, Hash>;

	/**
	 * @brief An unordered hash map from `Vector2` to `T`.
	 * @tparam T value mapped from `Vector2`
	 */
	template<typename T> using HashMap = std::unordered_map<Vector2, T, Hash>;

	std::string toString() const;
};

namespace Project {

	std::ostream& operator<<(std::ostream &outputStream, Vector2 const &vector);

}

#endif
