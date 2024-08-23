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
#include "application/linear_interpolation.hpp"
#include "application/param_info.hpp"
#include "application/main_loop.hpp"
#include "application/print.hpp"

#include <algorithm>
#include <cassert>
#include <random>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#ifdef __EMSCRIPTEN_PTHREADS__
#error "Should not use POSIX threads for this program."
#endif
#endif



int main(int const argc, char *argv[]) {
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
	int sleepTimeMilliseconds{
		App::ParamInfo::assertNonnegative(
			App::ParamInfo::castArg<int>(config.at("delay").argument),
			(std::ostringstream() << "Bad value for `delay`." ).str()
		)
	};
	bool const mazeWrap{App::ParamInfo::castArg<bool>(config.at("wrap").argument)};
	App::Performer::MazeType const mazeType{[gridType]() -> App::Performer::MazeType {
		/**/ if (gridType == "square") return App::Performer::MazeType::square;
		else if (gridType == "hexagon") return App::Performer::MazeType::hexagon;
		else App::errorExit("Unable to resolve grid type from string: `", gridType, "`.");
	}()};
	App::Performer::SearchType const searchType{[searchAlgorithmName]() -> App::Performer::SearchType {
		/**/ if (searchAlgorithmName == "depth") return App::Performer::SearchType::depth;
		else if (searchAlgorithmName == "breadth" or searchAlgorithmName == "dijkstra") return App::Performer::SearchType::breadth;
		else if (searchAlgorithmName == "greedy") return App::Performer::SearchType::greedy;
		else if (searchAlgorithmName == "a_star") App::errorExit("A Star is currently unsupported.");
		else App::errorExit("Unable to resolve graph search algorithm from string: `", searchAlgorithmName, "`.");
	}()};

	App::performer.emplace(mazeType, mazeSize, seed, mazeWrap, searchType, sleepTimeMilliseconds);

	// Print the parameter values.
	for (auto const &[name, param] : config) {
		App::println(name, ": ", param.argument);
	}

	// Initialize the Simple Directmedia Layer library.
	SDL_Init(SDL_INIT_VIDEO);

	#ifdef __EMSCRIPTEN__

	if (SDL_SetHintWithPriority(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas", SDL_HINT_OVERRIDE) == SDL_FALSE) {
		std::cerr << "Binding element for keyboard inputs was not set to canvas." << '\n';
	}

	#endif

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

	static constexpr char const *windowTitle{"Maze Engine"};

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

	#ifndef __EMSCRIPTEN__
	SDL_SetWindowMinimumSize(App::Window::window, 250, 150);
	#endif

	SDL_SetRenderDrawColor(App::Window::renderer, 0u, 0u, 0u, 1u);

	#ifdef __EMSCRIPTEN__
	/*
		Notify JavaScript that exported application functions can be called.
		Note: only use single quotes `'` instead of double quotes `"` when using `EM_ASM` macro.
	*/
	if (int const success{
			EM_ASM_INT(
				{
					if (typeof onMazeEngineApplicationInitialized === 'function') {
						onMazeEngineApplicationInitialized();
						return true; // success
					} else {
						return false; // failure
					}
				},
				/* no arguments */
			)
		};
		not success
	) {
		std::cerr <<
			"JavaScript function `onMazeEngineApplicationInitialized` was not called successfully." " "
			"Check if it's defined." "\n";
	}
	#endif

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
