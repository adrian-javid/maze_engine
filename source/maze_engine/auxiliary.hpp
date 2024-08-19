#ifndef MazeEngine_Auxiliary_hpp
#define MazeEngine_Auxiliary_hpp

#include <type_traits>
#include <cassert>

namespace MazeEngine::Aux {
	template<class T>
	[[nodiscard]] constexpr auto abs(T const& x) noexcept {
		static_assert(std::is_arithmetic_v<T>);
		return x < T{} ? -x : x;
	}

	template <typename IntT>
	constexpr std::enable_if_t<std::is_integral_v<IntT>, IntT>
	wrap(IntT value, IntT const upperBound) {
		assert(upperBound > IntT{0});
		value %= upperBound;
		if (value < IntT{0}) value += upperBound;
		assert(value >= IntT{0});
		return value;
	}

	template <typename FloatT>
	inline std::enable_if_t<std::is_floating_point_v<FloatT>, FloatT>
	wrap(FloatT value, FloatT const upperBound) {
		static constexpr FloatT zero{0.0};

		assert(upperBound != zero);
		assert(zero < upperBound);

		value = std::fmod(value, upperBound);

		if (value < zero) value += upperBound;

		if (value >= upperBound) value = zero;

		return value;
	}
}

#endif
