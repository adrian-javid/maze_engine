namespace App {/*


	This program solves mazes!


*/}

#include "maze_engine/search/breadth_first.hpp"
#include "maze_engine/search/depth_first.hpp"
#include "maze_engine/search/greedy_best_first.hpp"
#include "maze_engine/search/a_star.hpp"
#include "application/performer.hpp"

#include "application/simple_directmedia_layer.hpp"
#include "application/window.hpp"
#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "application/util.hpp"
#include "application/param_info.hpp"
#include "application/common.hpp"

#include <algorithm>
#include <cassert>
#include <random>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#ifdef __EMSCRIPTEN_PTHREADS__
#error "Should not use POSIX threads for this program."
#endif
#endif

#if false
namespace App {
	using namespace std::chrono_literals;
	static std::chrono::milliseconds sleepTime(0ms);
	static inline void delay() { std::this_thread::sleep_for(sleepTime); }
}
#endif

int main(int const argc, char *argv[]) {
	#if false
	// Maze engine.
	namespace Engine = MazeEngine;
	#endif

	// Parse arguments into key-value pairs.
	auto const &config = App::ParamInfo::parseArgv(argc, argv);

	// Get values from `config`.

	std::string_view const gridType(config.at("grid").argument);
	int const mazeSize{
		App::ParamInfo::assertPositive(
			App::ParamInfo::castArg<int>(config.at("size").argument),
		(std::ostringstream() << "Bad value for `size`." ).str())
	};
	unsigned int const seed{App::ParamInfo::castArg<unsigned int>(config.at("seed").argument)};
	std::string_view const searchAlgorithmName(config.at("search").argument);
	#if false
	App::sleepTime = std::chrono::milliseconds(
		App::ParamInfo::assertNonnegative(
			App::ParamInfo::castArg<int>(config.at("delay").argument),
		(std::ostringstream() << "Bad value for `delay`." ).str())
	);
	#else
	int sleepTimeMilliseconds{
		App::ParamInfo::assertNonnegative(
			App::ParamInfo::castArg<int>(config.at("delay").argument),
			(std::ostringstream() << "Bad value for `delay`." ).str()
		)
	};
	#endif
	bool const mazeWrap{App::ParamInfo::castArg<bool>(config.at("wrap").argument)};
	App::Performer::MazeType const mazeType{[gridType]() -> App::Performer::MazeType {
		/**/ if (gridType == "square") return App::Performer::MazeType::square;
		else if (gridType == "hexagon") return App::Performer::MazeType::hexagon;
		else App::Util::errOut("Unable to resolve grid type from string: `", gridType, "`.");
	}()};
	App::Performer::SearchType const searchType{[searchAlgorithmName]() -> App::Performer::SearchType {
		/**/ if (searchAlgorithmName == "depth") return App::Performer::SearchType::depth;
		else if (searchAlgorithmName == "breadth" or searchAlgorithmName == "dijkstra") return App::Performer::SearchType::breadth;
		else if (searchAlgorithmName == "greedy") return App::Performer::SearchType::greedy;
		else if (searchAlgorithmName == "a_star") App::Util::errOut("A Star is currently unsupported.");
		else App::Util::errOut("Unable to resolve graph search algorithm from string: `", searchAlgorithmName, "`.");
	}()};

	App::performer.emplace(mazeType, mazeSize, seed, mazeWrap, searchType, sleepTimeMilliseconds);

	#if false

	// The maze will be constructed with all walls before generating corridors.
	static constexpr Engine::Maze::Tile mazeFillValue{0xFFu};

	// Create maze object with grid type.
	if (gridType == "square") {
		App::maze = &(App::squareMaze = Engine::SquareMaze(mazeSize, mazeSize, mazeFillValue));
		if (mazeWrap)
			App::mazeEnd = {App::squareMaze.getRowCount() / 2, App::squareMaze.getColumnCount() / 2};
		else
			App::mazeEnd = {App::squareMaze.getRowCount() - 1, App::squareMaze.getColumnCount() - 1};
	} else if (gridType == "hexagon") {
		App::maze = &(App::hexagonMaze = Engine::HexagonMaze(mazeSize, mazeFillValue));
		App::mazeStart.value2 = -App::hexagonMaze.getRadius();
		if (not mazeWrap) App::mazeEnd.value2 = App::hexagonMaze.getRadius();
	} else {
		App::Util::errOut("Unable to resolve grid type from string: `", gridType, "`.");
	}

	// Generate the maze corridors.
	App::maze->generate(seed, mazeWrap);

	static constexpr auto const processVertex = [](Engine::Vector2 const &vertex) -> bool {
		/* lock */ {
			std::lock_guard const lock(App::tileInfoMutex);
			App::markedTileSet.insert(vertex);
		}
		App::delay();
		return vertex == App::mazeEnd;
	};

	static std::function<Engine::Vector2::HashMap<Engine::Vector2>(void)> searchMaze = nullptr;
	static std::variant<
		std::nullptr_t,
		Engine::DepthFirstSearchIterator,
		Engine::BreadthFirstSearchIterator,
		Engine::GreedyBestFirstSearchIterator
	> mazeSearchIteratorVariant;
	static Engine::MazeSearchIterator *mazeSearchIterator = nullptr;

	// Get the search algorithm.
	if (searchAlgorithmName == "depth") {
		searchMaze = []() { return depthFirstSearch(*App::maze, App::mazeStart, processVertex); };
		mazeSearchIteratorVariant = Engine::DepthFirstSearchIterator(*App::maze, App::mazeStart);
	} else if (searchAlgorithmName == "breadth" or searchAlgorithmName == "dijkstra") {
		searchMaze = []() { return breadthFirstSearch(*App::maze, App::mazeStart, processVertex); };
		mazeSearchIteratorVariant = Engine::BreadthFirstSearchIterator(*App::maze, App::mazeStart);
	} else if (searchAlgorithmName == "greedy") {
		searchMaze = []() { return greedyBestFirstSearch(*App::maze, App::mazeStart, App::mazeEnd, processVertex); };
		mazeSearchIteratorVariant = Engine::GreedyBestFirstSearchIterator(*App::maze, App::mazeStart, App::mazeEnd);
	} else if (searchAlgorithmName == "a_star") {
		searchMaze = []() { return aStarSearch(*App::maze, App::mazeStart, App::mazeEnd, processVertex); };
		mazeSearchIteratorVariant = nullptr;
	} else {
		App::Util::errOut("Unable to resolve graph search algorithm from string: `", searchAlgorithmName, "`.");
	}
	assert(searchMaze != nullptr);
	assert(not std::holds_alternative<std::nullptr_t>(mazeSearchIteratorVariant));

	mazeSearchIterator = std::visit(
		[](auto &iterator) -> Engine::MazeSearchIterator *{
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

		App::Util::synchronizedPrint((std::ostringstream() << "Explored count: " << App::markedTileSet.size()).str());

		// Path tiles.
		for (
			auto edge(upTree.find(App::mazeEnd));
			edge->first /* child vertex */ != App::mazeStart;
			edge = upTree.find(edge->second /* parent vertex */)
		) {
			/* lock */ {
				std::lock_guard const lock(App::tileInfoMutex);
				App::pathTileSet.insert(edge->first);
			}
			App::delay();
		}

		/* lock */ {
			std::lock_guard const lock(App::tileInfoMutex);
			App::pathTileSet.insert(App::mazeStart); // include corner
		}

		App::Util::synchronizedPrint((std::ostringstream() << "Path length: " << App::pathTileSet.size()).str());
	};

	#endif

	// Print the parameter values.
	for (auto const &[name, param] : config) {
		App::println(name, ": ", param.argument);
	}

	// Initialize the Simple Directmedia Layer library.
	SDL_Init(SDL_INIT_VIDEO);

	/*
		Note to self:

		I don't understand why calling this before `SDL_Init` causes a segmentation fault;
		I believe it has something to do with `SDL_Quit`.
	*/
	// Register exit handler.
	std::atexit(+[]() -> void {
		if (App::Window::window) SDL_DestroyWindow(App::Window::window);
		if (App::Window::renderer) SDL_DestroyRenderer(App::Window::renderer);
		SDL_Quit();
	});

	static constexpr char const *windowTitle = "Maze Engine";

	App::Window::window = SDL_CreateWindow(
		windowTitle,
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		App::Window::windowWidth, App::Window::windowHeight,
		SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
	);

	// Assert window was successfully created.
	assert(App::Window::window != nullptr);

	/*
		The software renderer supports VSync, so can always fallback on software renderer
		unless not using SDL renderers.
	*/
	App::Window::renderer = SDL_CreateRenderer(App::Window::window, -1, SDL_RENDERER_PRESENTVSYNC);

	if (App::Window::renderer == nullptr)
		App::Window::renderer = SDL_CreateRenderer(App::Window::window, -1, 0u);

	// Assert renderer was successfully created.
	assert(App::Window::renderer != nullptr);

	SDL_SetWindowTitle(App::Window::window, windowTitle);
	SDL_SetWindowMinimumSize(App::Window::window, 250, 150);

	#if false
	// Start worker thread.
	std::thread const mazeSolver(solveMaze);
	#endif

	SDL_SetRenderDrawColor(App::Window::renderer, 0u, 0u, 0u, 1u);

	// Start the main loop.
	#ifdef __EMSCRIPTEN__
	/*
		Note to self:

		`simulate_infinite_loop` is `true`, so will not continue execution after this function ends.

		"...if simulate_infinite_loop is false, and you created an object on the stack,
		it will be cleaned up before the main loop is called for the first time.""
		(https://emscripten.org/docs/api_reference/emscripten.h.html#id3)
	*/
	emscripten_set_main_loop(&App::mainLoop, -1, true);
	#else
	while (true) App::mainLoop();
	#endif

	return EXIT_SUCCESS;
}
