#include "UnionFinder.hpp"

Project::UnionFinder::UnionFinder(std::size_t const count): forest(count, -1) {}

void Project::UnionFinder::addElements(std::size_t const count) {
	forest.insert(forest.end(), count, -1);
}

auto Project::UnionFinder::find(Identifier const element) -> Identifier {
	// base case
	if (forest.at(element) < 0) return element;

	// recursion
	Identifier const root{find(forest.at(element))};

	// path compression
	forest.at(element) = root;

	return root;
}

void Project::UnionFinder::unionThem(Identifier const element1, Identifier const element2) {
	Identifier const root1 = find(element1);
	Identifier const root2 = find(element2);
	Identifier const negativeSize = forest.at(root1) + forest.at(root2);

	if (forest.at(root1) < forest.at(root2)) {
		forest.at(root2) = root1;
		forest.at(root1) = negativeSize;
	} else {
		forest.at(root1) = root2;
		forest.at(root2) = negativeSize;
	}
}

auto Project::UnionFinder::size(Identifier const element) -> Identifier {
	Identifier const root = find(element);
	return -1 * forest.at(root);
}
