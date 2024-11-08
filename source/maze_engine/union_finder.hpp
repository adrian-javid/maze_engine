#ifndef MazeEngine_UnionFinder_hpp
#define MazeEngine_UnionFinder_hpp

#include <vector>

namespace MazeEngine { class UnionFinder; }

class MazeEngine::UnionFinder {
	public:

		using Identifier = int;
		static_assert(std::is_signed_v<Identifier>);
		static_assert(std::is_integral_v<Identifier>);

		class View {
			public:
				[[nodiscard]] constexpr explicit View(UnionFinder &paramUnionFinder):
					unionFinder{&paramUnionFinder} {};

				[[nodiscard]] inline Identifier find(Identifier const element) const {
					return unionFinder->find(element);
				}

				[[nodiscard]] inline Identifier getSize(Identifier const element) const {
					return unionFinder->getSize(element);
				}

			private:
				UnionFinder *unionFinder;
		};

	private:

		std::vector<Identifier> forest;

	public:

		[[nodiscard]]
		UnionFinder(std::size_t const count=0u);

		void addElements(std::size_t const count);

		void unionThem(Identifier const element1, Identifier const element2);

		[[nodiscard]]
		Identifier find(Identifier const element);

		[[nodiscard]]
		Identifier getSize(Identifier const element);

};

#endif
