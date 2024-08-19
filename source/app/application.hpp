#ifndef app_APPLICATION_HPP
#define app_APPLICATION_HPP

#include "simple_directmedia_layer.hpp"
#include <thread>
#include "core/maze.hpp"
#include "core/maze/square.hpp"
#include "core/maze/hexagon.hpp"
#include "core/vector2.hpp"

// Application stuff.
namespace App {
	extern MazeEngine::Maze *maze;
	extern MazeEngine::SquareMaze squareMaze;
	extern MazeEngine::HexagonMaze hexagonMaze;
	extern MazeEngine::Vector2 mazeStart;
	extern MazeEngine::Vector2 mazeEnd;

	extern std::chrono::milliseconds sleepTime;
	inline void delay() { std::this_thread::sleep_for(sleepTime); }

	extern std::mutex tileInfoMutex;
	extern MazeEngine::Vector2::HashSet pathTileSet;
	extern MazeEngine::Vector2::HashSet markedTileSet;

	// Change in time.
	extern Uint64 deltaTime;

	void mainLoop();
}

#endif
