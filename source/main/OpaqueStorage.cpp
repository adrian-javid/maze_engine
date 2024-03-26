#include "OpaqueStorage.hpp"

using namespace Project;

OpaqueStorage::OpaqueStorage(
    std::function<void(Vector2 const &)> functionPush,
    std::function<Vector2()> functionPop
):
    functionPush(functionPush),
    functionPop(functionPop)
{}

void OpaqueStorage::push(Vector2 const &vector) { functionPush(vector); }

Vector2 OpaqueStorage::pop() { return std::move(functionPop()); }
