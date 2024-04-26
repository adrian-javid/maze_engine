#include "UnionFinder.hpp"

Project::UnionFinder::UnionFinder(std::size_t const count): forest(count, -1) {}

void Project::UnionFinder::addElements(std::size_t const count) {
    forest.insert(forest.end(), count, -1);
}

int Project::UnionFinder::find(int const element) {
    // base case
    if (forest.at(element) < 0) return element;

    // recursion
    int const root{find(forest.at(element))};

    // path compression
    forest.at(element) = root;

    return root;
}

void Project::UnionFinder::unionThem(int const element1, int const element2) {
  int const root1 = find(element1);
  int const root2 = find(element2);
  int const negativeSize = forest.at(root1) + forest.at(root2);

  if (forest.at(root1) < forest.at(root2)) {
    forest.at(root2) = root1;
    forest.at(root1) = negativeSize;
  } else {
    forest.at(root1) = root2;
    forest.at(root2) = negativeSize;
  }
}

int Project::UnionFinder::size(int const element) {
    int const root = find(element);
    return -1 * forest.at(root);
}
