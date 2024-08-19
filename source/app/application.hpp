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
	extern Project::Maze *maze;
	extern Project::SquareMaze squareMaze;
	extern Project::HexagonMaze hexagonMaze;
	extern Project::Vector2 mazeStart;
	extern Project::Vector2 mazeEnd;

	extern std::chrono::milliseconds sleepTime;
	inline void delay() { std::this_thread::sleep_for(sleepTime); }

	extern std::mutex tileInfoMutex;
	extern Project::Vector2::HashSet pathTileSet;
	extern Project::Vector2::HashSet markedTileSet;

	// Change in time.
	extern Uint64 deltaTime;

	void mainLoop();
}

#endif
