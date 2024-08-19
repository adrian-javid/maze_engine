#include "application.hpp"

namespace App {
	using namespace std::chrono_literals;

	Project::Maze *maze = nullptr;
	Project::SquareMaze squareMaze;
	Project::HexagonMaze hexagonMaze;
	Project::Vector2 mazeStart(0, 0);
	Project::Vector2 mazeEnd(0, 0);

	std::chrono::milliseconds sleepTime = 0ms;

	std::mutex tileInfoMutex;
	Project::Vector2::HashSet pathTileSet;
	Project::Vector2::HashSet markedTileSet;

	// Change in time.
	Uint64 deltaTime{0u};
}