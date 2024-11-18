#ifndef Application_UniformIntDistribution_hpp
#define Application_UniformIntDistribution_hpp

#include <random>

/*
	This is simply a wrapper around
	`std::uniform_int_distribution` to make it simpler to use.
*/
namespace App { template <typename> class UniformIntDistribution; }

template <typename IntT>
class App::UniformIntDistribution {
	public:
		[[nodiscard]] inline IntT getInt() {
			return uniformIntDistribution(randomNumberGenerator);
		}

		struct Range { IntT lowerBound{}, upperBound{}; };

		[[nodiscard]] explicit
		UniformIntDistribution() = delete;

		[[nodiscard]] explicit
		UniformIntDistribution(unsigned int const seed, Range const range):
			randomNumberGenerator(seed),
			uniformIntDistribution(range.lowerBound, range.upperBound)
		{}

	private:
		std::mt19937 randomNumberGenerator = {};
		std::uniform_int_distribution<IntT> uniformIntDistribution;
};

#endif