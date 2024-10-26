#include "maze_engine/union_finder.hpp"

#include <cassert>

MazeEngine::UnionFinder::UnionFinder(std::size_t const count): forest(count, -1) {}

void MazeEngine::UnionFinder::addElements(std::size_t const count) {
	forest.insert(forest.end(), count, -1);
}

auto MazeEngine::UnionFinder::find(Identifier const element) -> Identifier {
	assert(element >= 0);

	// base case
	if (forest.at(static_cast<std::size_t>(element)) < 0) return element;

	// recursion
	Identifier const root{find(forest.at(static_cast<std::size_t>(element)))};

	// path compression
	forest.at(static_cast<std::size_t>(element)) = root;

	return root;
}

void MazeEngine::UnionFinder::unionThem(Identifier const element1, Identifier const element2) {
	assert(element1 >= 0);
	assert(element2 >= 0);

	Identifier const root1{find(element1)};
	Identifier const root2{find(element2)};

	Identifier const negativeSize{forest.at(root1) + forest.at(root2)};

	if (forest.at(root1) < forest.at(root2)) {
		forest.at(root2) = root1;
		forest.at(root1) = negativeSize;
	} else {
		forest.at(root1) = root2;
		forest.at(root2) = negativeSize;
	}
}

auto MazeEngine::UnionFinder::getSize(Identifier const element) -> Identifier {
	Identifier const root{find(element)};
	return -1 * forest.at(root);
}
