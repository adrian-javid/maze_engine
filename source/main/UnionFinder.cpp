#include "UnionFinder.hpp"

int Project::UnionFinder::find(int const element) {
    // base case
    if (forest.at(element) < 0) return element;

    // recursion
    int const root{find(forest.at(element))};

    // path compression
    forest.at(element) = root;

    return root;
}
