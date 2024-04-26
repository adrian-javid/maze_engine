#ifndef UnionFinder_hpp
#define UnionFinder_hpp

#include <vector>

namespace Project { class UnionFinder; }

class Project::UnionFinder {
  private:
    std::vector<int> forest;

  public:

    UnionFinder(std::size_t const count=0u);

    void addElements(std::size_t const count);

    void unionThem(int const element1, int const element2);

    int find(int const element);

    int size(int const element);

};

#endif
