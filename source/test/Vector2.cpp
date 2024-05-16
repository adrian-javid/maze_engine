#include "catch2/catch_amalgamated.hpp"
#include "Vector2.hpp"
#include "SquareMaze.hpp"

using namespace Project;

TEST_CASE("Neighbors", "[Vector2]") {
    static constexpr int rowCount = 20;
    static constexpr int columnCount = 20;

    Vector2 const vector(1, 1);
    CHECK(vector == Vector2(1, 1));

    Vector2 const northNeighbor = (vector + SquareMaze::northOffset).wrap(rowCount, columnCount);
    CHECK(northNeighbor == Vector2(0, 1));

    Vector2 const southNeighbor = (vector + SquareMaze::southOffset).wrap(rowCount, columnCount);
    CHECK(southNeighbor == Vector2(2, 1));

    Vector2 const eastNeighbor = (vector + SquareMaze::eastOffset).wrap(rowCount, columnCount);
    CHECK(eastNeighbor == Vector2(1, 2));

    Vector2 const westNeighbor = (vector + SquareMaze::westOffset).wrap(rowCount, columnCount);
    CHECK(westNeighbor == Vector2(1, 0));

}
