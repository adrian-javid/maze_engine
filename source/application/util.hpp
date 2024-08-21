#ifndef Application_Util_hpp
#define Application_Util_hpp

#include <cmath>
#include <cassert>
#include <type_traits>
#include <functional>
#include <string>
#include "maze_engine/auxiliary.hpp"
#include "macros.hpp"

namespace App::Util {

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

	FORCE_INLINE inline
	double percentageWrap(double const value) { return MazeEngine::Aux::wrap(value, 1.00); }

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

	[[deprecated]]
	inline void synchronizedPrint(std::string const &message, char const end='\n') {
		static std::mutex mutex;
		std::lock_guard const lock(mutex);
		std::cout << message << end;
	}

	[[deprecated]]
	inline void print(std::string const &message, char const end='\n') {
		std::cout << message << end;
	}

	/*
		Print message with trailing newline character, then exit with failure code.
	*/
	template<typename... ParamsT>
	[[noreturn]] FORCE_INLINE
	void errOut(ParamsT &&...args) {
		(std::cerr << ... << std::forward<ParamsT>(args)) << '\n';
		std::exit(EXIT_FAILURE);
	}
}

#endif
