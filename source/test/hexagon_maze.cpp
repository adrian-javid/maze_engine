#include "catch2/catch_amalgamated.hpp"

#include "maze_engine/maze/hexagon.hpp"

#include <array>

TEST_CASE("Hexagon Ring", "[HexagonMaze]") {

	MazeEngine::HexagonMaze maze(3);

	std::size_t keyCount{0u};

	SECTION("lone center vector: ring radius 0") {
		static constexpr MazeEngine::Vector2 const center{0, 0};
		static constexpr MazeEngine::Vector2::Value const ringRadius{0};

		maze.forEachKeyInRing(center, ringRadius, [&keyCount](MazeEngine::Vector2 const key) {
			CHECK(key == center);
			++keyCount;
			REQUIRE(keyCount <= 1);
		});

		CHECK(keyCount == 1u);
	}

	SECTION("simple vectors: ring radius 1") {
		static constexpr MazeEngine::Vector2 const center{0, 0};
		static constexpr MazeEngine::Vector2::Value const ringRadius{1};

		maze.forEachKeyInRing(center, ringRadius, [&keyCount](MazeEngine::Vector2 const key) -> void {
			INFO("key in ring: " << key);
			CHECK(MazeEngine::HexagonMaze::isSimpleOffsetVector(key));
			++keyCount;
			REQUIRE(keyCount <= 6u);
		});

		CHECK(keyCount == 6u);
	}

}
