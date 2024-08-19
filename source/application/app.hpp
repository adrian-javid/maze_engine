#ifndef Application_App_hpp
#define Application_App_hpp

#include "simple_directmedia_layer.hpp"
#include <thread>
#include "maze_engine/maze.hpp"
#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "maze_engine/vector2.hpp"
#include <mutex>

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
