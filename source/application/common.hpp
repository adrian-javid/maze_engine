#ifndef Application_App_hpp
#define Application_App_hpp

#include "simple_directmedia_layer.hpp"

// Application stuff.
namespace App {
	#if false
	extern MazeEngine::Maze *maze;
	extern MazeEngine::SquareMaze squareMaze;
	extern MazeEngine::HexagonMaze hexagonMaze;
	extern MazeEngine::Vector2 mazeStart;
	extern MazeEngine::Vector2 mazeEnd;

	extern std::mutex tileInfoMutex;
	extern MazeEngine::Vector2::HashSet pathTileSet;
	extern MazeEngine::Vector2::HashSet markedTileSet;
	#endif

	// Change in time.
	extern Uint64 deltaTime;

	void mainLoop();
}


#endif
