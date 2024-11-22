#include "catch2/catch_amalgamated.hpp"

#include "maze_engine/maze/hexagon.hpp"

#include <array>
#include <unordered_set>

TEST_CASE("Hexagon Ring", "[HexagonMaze]") {

	MazeEngine::HexagonMaze maze(3);


	SECTION("lone center vector: ring index 0") {
		static constexpr MazeEngine::Vector2 const center{0, 0};
		static constexpr MazeEngine::Vector2::Value const ringIndex{0};

		std::size_t keyCount{0u};

		maze.forEachKeyInRing(center, ringIndex, [&keyCount](MazeEngine::Vector2 const key) {
			CHECK(key == center);
			++keyCount;
			REQUIRE(keyCount <= 1);
		});

		CHECK(keyCount == 1u);
	}

	SECTION("simple vectors: ring index 1") {
		static constexpr MazeEngine::Vector2 const center{0, 0};
		static constexpr MazeEngine::Vector2::Value const ringIndex{1};

		MazeEngine::Vector2::HashSet obtainedKeys;

		maze.forEachKeyInRing(center, ringIndex, [&obtainedKeys](MazeEngine::Vector2 const key) -> void {
			INFO("key in ring: " << key);
			CHECK(MazeEngine::HexagonMaze::isSimpleOffsetVector(key));
			obtainedKeys.insert(key);
			REQUIRE(obtainedKeys.size() <= 6u);
		});

		CHECK(obtainedKeys.size() == 6u);
	}

}
