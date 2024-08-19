#include "application/app.hpp"

namespace App {
	using namespace std::chrono_literals;

	MazeEngine::Maze *maze = nullptr;
	MazeEngine::SquareMaze squareMaze;
	MazeEngine::HexagonMaze hexagonMaze;
	MazeEngine::Vector2 mazeStart(0, 0);
	MazeEngine::Vector2 mazeEnd(0, 0);

	std::mutex tileInfoMutex;
	MazeEngine::Vector2::HashSet pathTileSet;
	MazeEngine::Vector2::HashSet markedTileSet;

	// Change in time.
	Uint64 deltaTime{0u};
}