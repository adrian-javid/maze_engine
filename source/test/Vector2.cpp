#include "catch2/catch_amalgamated.hpp"
#include "Vector2.hpp"

using namespace Project;

TEST_CASE("Neighbors", "[Vector2]") {
    int constexpr rowCount = 20; int constexpr columnCount = 20;

    Vector2 const vector(1, 1);
    CHECK(vector == Vector2(1, 1));

    Vector2 const northNeighbor = (vector + Vector2_NORTH).wrap(rowCount, columnCount);
    CHECK(northNeighbor == Vector2(0, 1));

    Vector2 const southNeighbor = (vector + Vector2_SOUTH).wrap(rowCount, columnCount);
    CHECK(southNeighbor == Vector2(2, 1));

    Vector2 const eastNeighbor = (vector + Vector2_EAST).wrap(rowCount, columnCount);
    CHECK(eastNeighbor == Vector2(1, 2));

    Vector2 const westNeighbor = (vector + Vector2_WEST).wrap(rowCount, columnCount);
    CHECK(westNeighbor == Vector2(1, 0));

}
