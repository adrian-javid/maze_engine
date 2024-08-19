namespace MazeEngine::Global {/*


	This program solves mazes!


*/}

#include "maze_engine/search/breadth_first.hpp"
#include "maze_engine/search/depth_first.hpp"
#include "maze_engine/search/greedy_best_first.hpp"
#include "maze_engine/search/a_star.hpp"

#include "application/simple_directmedia_layer.hpp"
#include "application/window.hpp"
#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "application/util.hpp"
#include "application/app_param.hpp"
#include "application/application.hpp"

#include <thread>
#include <mutex>
#include <chrono>
#include <variant>

#include <algorithm>
#include <cassert>
#include <random>
#ifdef __EMSCRIPTEN__
#ifndef __EMSCRIPTEN_PTHREADS__
#error "POSIX threads are not enabled."
#endif
#include <emscripten.h>
#endif

int main(int const argc, char *argv[]) {
	using namespace App;
	using namespace MazeEngine;

	auto const &config = AppParamInfo::parseArgv(argc, argv);

	// Get values from `config`.

	std::string const &gridType = config.at("grid").argument;
	int const mazeSize{
		AppParamInfo::assertPositive(
			AppParamInfo::castArg<int>(config.at("size").argument),
		(std::ostringstream() << "Bad value for `size`." ).str())
	};
	unsigned int const seed{AppParamInfo::castArg<unsigned int>(config.at("seed").argument)};
	std::string const &searchAlgorithmName = config.at("search").argument;
	App::sleepTime = std::chrono::milliseconds(
		AppParamInfo::assertNonnegative(
			AppParamInfo::castArg<int>(config.at("delay").argument),
		(std::ostringstream() << "Bad value for `delay`." ).str())
	);
	bool const mazeWrap = AppParamInfo::castArg<bool>(config.at("wrap").argument);

	static constexpr int mazeFillValue{0xFFu};

	// Create maze object with grid type.
	if (gridType == "square") {
		App::maze = &(App::squareMaze = SquareMaze(mazeSize, mazeSize, mazeFillValue));
		if (mazeWrap)
			App::mazeEnd = {App::squareMaze.getRowCount() / 2, App::squareMaze.getColumnCount() / 2};
		else
			App::mazeEnd = {App::squareMaze.getRowCount() - 1, App::squareMaze.getColumnCount() - 1};
	} else if (gridType == "hexagon") {
		App::maze = &(App::hexagonMaze = HexagonMaze(mazeSize, mazeFillValue));
		App::mazeStart.value2 = -App::hexagonMaze.getRadius();
		if (not mazeWrap) App::mazeEnd.value2 = App::hexagonMaze.getRadius();
	} else {
		Util::errOut("Unable to resolve grid type from string: `" + gridType + "`.");
	}

	// Generate the maze corridors.
	App::maze->generate(seed, mazeWrap);

	static std::size_t exploredCount{0u};
	static constexpr auto const processVertex = [](Vector2 const &vertex) -> bool {
		++exploredCount;
		/* lock */ {
			std::lock_guard const lock(App::tileInfoMutex);
			App::markedTileSet.insert(vertex);
		}
		App::delay();
		return vertex == App::mazeEnd;
	};

	static std::function<Vector2::HashMap<Vector2>(void)> searchMaze = nullptr;
	static std::variant<std::nullptr_t, DepthFirstSearchIterator, BreadthFirstSearchIterator, GreedyBestFirstSearchIterator> mazeSearchIteratorVariant;
	static MazeSearchIterator *mazeSearchIterator = nullptr;

	// Get the search algorithm.
	if (searchAlgorithmName == "depth") {
		searchMaze = []() { return depthFirstSearch(*App::maze, App::mazeStart, processVertex); };
		mazeSearchIteratorVariant = DepthFirstSearchIterator(*App::maze, App::mazeStart);
	} else if (searchAlgorithmName == "breadth" or searchAlgorithmName == "dijkstra") {
		searchMaze = []() { return breadthFirstSearch(*App::maze, App::mazeStart, processVertex); };
		mazeSearchIteratorVariant = BreadthFirstSearchIterator(*App::maze, App::mazeStart);
	} else if (searchAlgorithmName == "greedy") {
		searchMaze = []() { return greedyBestFirstSearch(*App::maze, App::mazeStart, App::mazeEnd, processVertex); };
		mazeSearchIteratorVariant = GreedyBestFirstSearchIterator(*App::maze, App::mazeStart, App::mazeEnd);
	} else if (searchAlgorithmName == "a_star") {
		searchMaze = []() { return aStarSearch(*App::maze, App::mazeStart, App::mazeEnd, processVertex); };
		mazeSearchIteratorVariant = nullptr;
	} else {
		Util::errOut("Unable to resolve graph search algorithm from string: `" + searchAlgorithmName + "`.");
	}
	assert(searchMaze != nullptr);
	assert(not std::holds_alternative<std::nullptr_t>(mazeSearchIteratorVariant));

	static std::size_t pathLength{0u};

	mazeSearchIterator = std::visit(
		[](auto &iterator) -> MazeSearchIterator *{
			if constexpr (std::is_same_v<decltype(iterator), std::nullptr_t &>)
				return nullptr;
			else
				return &iterator;
		},
		mazeSearchIteratorVariant
	);

	assert(mazeSearchIterator != nullptr);

	static constexpr auto solveMaze = []() -> void {
		// Search for end of maze.

		for (; not mazeSearchIterator->isEnd(); ++*mazeSearchIterator) {
			if (processVertex(**mazeSearchIterator)) break;
		}

		auto const upTree(mazeSearchIterator->getHistory());

		Util::synchronizedPrint((std::ostringstream() << "Explored count: " << exploredCount).str());

		// Path tiles.
		for (
			auto edge(upTree.find(App::mazeEnd));
			edge->first /* child vertex */ != App::mazeStart;
			edge = upTree.find(edge->second /* parent vertex */)
		) {
			++pathLength;
			/* lock */ {
				std::lock_guard const lock(App::tileInfoMutex);
				App::pathTileSet.insert(edge->first);
			}
			App::delay();
		}

		++pathLength;
		/* lock */ {
			std::lock_guard const lock(App::tileInfoMutex);
			App::pathTileSet.insert(App::mazeStart); // include corner
		}

		Util::synchronizedPrint((std::ostringstream() << "Path length: " << pathLength).str());
	};

	std::ostringstream outputStream;
	for (auto const &[name, param] : config) {
		outputStream.str(std::string());
		outputStream << name << ": " << param.argument << '\n';
		Util::synchronizedPrint(outputStream.str(), '\0');
	}

	// Initialize the Simple Directmedia Layer library.
	SDL_Init(SDL_INIT_VIDEO);

	// Register exit handler.
	/*
		Note to self:

		I don't understand why calling this before `SDL_Init` causes a segmentation fault.
		I believe it has something to do with `SDL_Quit`.
	*/
	std::atexit(+[]() -> void {
		if (Window::window) SDL_DestroyWindow(Window::window);
		if (Window::renderer) SDL_DestroyRenderer(Window::renderer);
		SDL_Quit();
	});

	static constexpr char const *windowTitle = "Maze Engine";

	Window::window = SDL_CreateWindow(
		windowTitle,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Window::windowWidth, Window::windowHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	// Assert window was successfully created.
	assert(Window::window != nullptr);

	/*
		The software renderer supports VSync, so can always fallback on software renderer
		unless not using SDL renderers.
	*/
	Window::renderer = SDL_CreateRenderer(Window::window, -1, SDL_RENDERER_PRESENTVSYNC);

	if (Window::renderer == nullptr)
		Window::renderer = SDL_CreateRenderer(Window::window, -1, 0u);

	// Assert renderer was successfully created.
	assert(Window::renderer != nullptr);

	SDL_SetWindowTitle(Window::window, windowTitle);
	SDL_SetWindowMinimumSize(Window::window, 250, 150);

	// Set the window refresher. This is called every iteration in the main loop.
	Window::windowRefresher = &Window::refresh;

	// Start worker thread.
	std::thread const mazeSolver(solveMaze);

	SDL_SetRenderDrawColor(Window::renderer, 0u, 0u, 0u, 1u);

	// Start the main loop.
	#ifdef __EMSCRIPTEN__
	/*
		Note to self:

		`simulate_infinite_loop` is `true`, so will not continue execution after this function ends.

		"...if simulate_infinite_loop is false, and you created an object on the stack,
		it will be cleaned up before the main loop is called for the first time.""
		(https://emscripten.org/docs/api_reference/emscripten.h.html#id3)
	*/
	emscripten_set_main_loop(&mainLoop, -1, true);
	#else
	while (true) mainLoop();
	#endif

	return EXIT_SUCCESS;
}
