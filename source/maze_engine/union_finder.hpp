#ifndef MazeEngine_UnionFinder_hpp
#define MazeEngine_UnionFinder_hpp

#include <vector>

namespace MazeEngine { class UnionFinder; }

class MazeEngine::UnionFinder {
	public:

		using Identifier = int;
		static_assert(std::is_signed_v<Identifier>);
		static_assert(std::is_integral_v<Identifier>);

	private:

		std::vector<Identifier> forest;

	public:

		UnionFinder(std::size_t const count=0u);

		void addElements(std::size_t const count);

		void unionThem(Identifier const element1, Identifier const element2);

		Identifier find(Identifier const element);

		Identifier getSize(Identifier const element);

};

#endif
