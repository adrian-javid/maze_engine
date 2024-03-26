#ifndef OpaqueStorage_hpp 
#define OpaqueStorage_hpp

#include <functional>
#include <Vector2.hpp>

namespace Project { class OpaqueStorage; }

class Project::OpaqueStorage {

  private:

    std::function<void(Vector2 const &)> functionPush;

    std::function<Vector2()> functionPop;

  public:

    OpaqueStorage(
      std::function<void(Vector2 const &)> pushFunction,
      std::function<Vector2()> popFunction
    );

    void push(Vector2 const &vector);

    Vector2 pop();

};

#endif
