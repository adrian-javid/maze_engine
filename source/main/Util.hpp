#ifndef main_UTIL_HPP
#define main_UTIL_HPP

#include <cmath>
#include <cassert>
#include <type_traits>
#include <functional>
#include <any>
#include <string>
#include <iostream>
#include <mutex>

namespace Project::Util {

	template<typename T>
	class [[deprecated]] OpaqueIterator {
		private:
			std::function<T(std::any const &)> dereference;
			std::function<void(std::any &)> increment;
			std::function<bool(std::any const &, std::any const &)> equals;
			std::any baseIterator;
		public:
			template<typename BaseIterator_T>
			explicit OpaqueIterator(BaseIterator_T &&iterator):
				dereference([](std::any const &memory) -> T {
					return *std::any_cast<BaseIterator_T const &>(memory);
				}),
				increment([](std::any &memory) -> void {
					++std::any_cast<BaseIterator_T &>(memory);
				}),
				equals([](std::any const &memory1, std::any const &memory2) -> bool {
					return std::any_cast<BaseIterator_T const &>(memory1) == std::any_cast<BaseIterator_T const &>(memory2);
				}),
				baseIterator(iterator)
			{}

			T const &operator*() const { return dereference(baseIterator); }
			OpaqueIterator &operator++() { increment(baseIterator); return *this; }
			bool operator==(OpaqueIterator const &otherIterator) const { return equals(baseIterator, otherIterator.baseIterator); }
			bool operator!=(OpaqueIterator const &otherIterator) const { return not (*this == otherIterator); }
	};

	template<class T>
	[[nodiscard]] constexpr auto abs(T const& x) noexcept {
		static_assert(std::is_arithmetic_v<T>);
		return x < T{} ? -x : x;
	}

	constexpr int wrapValue(int value, int const upperBound) {
		assert(upperBound > 0);
		value %= upperBound;
		if (value < 0) value += upperBound;
		assert(value >= 0);
		return value;
	}

	double wrapValue(double value, double const upperBound);

	constexpr float linearInterpolation(float const percentage, float const start, float const end) {
		assert(percentage >= 0.0f);
		assert(percentage <= 1.0f);
		return start * (1.0f - percentage) + end * percentage;
	}

	constexpr double linearInterpolation(double const percentage, double const start, double const end) {
		assert(percentage >= 0.0);
		assert(percentage <= 1.0);
		return start * (1.0 - percentage) + end * percentage;
	}

	inline void synchronizedPrint(std::string const &message, char const end='\n') {
		static std::mutex mutex;
		std::lock_guard const lock(mutex);
		std::cout << message << end;
	}

	inline void print(std::string const &message, char const end='\n') {
		std::cout << message << end;
	}

	[[noreturn]]
	inline void errOut(std::string const &message, char const end='\n') {
		std::cerr << message << end;
		std::exit(EXIT_FAILURE);
	}
}

#endif
