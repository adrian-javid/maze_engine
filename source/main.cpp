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
#include "application/sound_table.hpp"
#include "application/audio_data.hpp"

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
	auto const &config(App::ParamInfo::parseArgv(argc, argv));

	// Get values from `config`.

	std::string_view const gridType(config.at("grid").argument);
	int const mazeSize{
		App::ParamInfo::assertPositive(
			App::ParamInfo::castArg<int>(config.at("size").argument),
		(std::ostringstream() << "Bad value for `size`." ).str())
	};
	unsigned int const seed{App::ParamInfo::castArg<unsigned int>(config.at("seed").argument)};
	std::string_view const searchAlgorithmName(config.at("search").argument);
	std::string_view const soundTypeName(config.at("sound").argument);
	int sleepTimeMilliseconds{
		App::ParamInfo::assertNonnegative(
			App::ParamInfo::castArg<int>(config.at("delay").argument),
			(std::ostringstream() << "Bad value for `delay`." ).str()
		)
	};
	bool const mazeWrap{App::ParamInfo::castArg<bool>(config.at("wrap").argument)};
	App::Performer::MazeType const mazeType{<:gridType:>() -> App::Performer::MazeType {
		/**/ if (gridType == "square") return App::Performer::MazeType::square;
		else if (gridType == "hexagon") return App::Performer::MazeType::hexagon;
		else App::errorExit("Unable to resolve grid type from string: `", gridType, "`.");
	}()};
	App::Performer::SearchType const searchType{<:searchAlgorithmName:>() -> App::Performer::SearchType {
		/**/ if (searchAlgorithmName == "depth") return App::Performer::SearchType::depth;
		else if (searchAlgorithmName == "breadth" or searchAlgorithmName == "dijkstra") return App::Performer::SearchType::breadth;
		else if (searchAlgorithmName == "greedy") return App::Performer::SearchType::greedy;
		else if (searchAlgorithmName == "a_star") App::errorExit("A Star is currently unsupported.");
		else App::errorExit("Unable to resolve graph search algorithm from string: `", searchAlgorithmName, "`.");
	}()};
	App::Performer::SoundType const soundType{<:soundTypeName:>() -> App::Performer::SoundType {
		/**/ if (soundTypeName == "none") return App::Performer::SoundType::none;
		else if (soundTypeName == "piano") return App::Performer::SoundType::piano;
		else if (soundTypeName == "synthesizer") return App::Performer::SoundType::synthesizer;
		else App::errorExit("Unable to resolve sound instrument frmo string: `", soundTypeName, "`.");
	}()};

	App::performer.emplace(mazeType, mazeSize, seed, mazeWrap, searchType, soundType, sleepTimeMilliseconds);

	// Print the parameter values.
	for (auto const &<:name, param:> : config) {
		App::println(name, ": ", param.argument);
	}

	// Initialize the Simple Directmedia Layer library.
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
		App::errorExit("SDL failed to initialize.");
	}

	#ifdef __EMSCRIPTEN__
	if (SDL_SetHintWithPriority(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas", SDL_HINT_OVERRIDE) == SDL_FALSE) {
		std::cerr << "Binding element for keyboard inputs was not set to canvas." << '\n';
	}
	#endif

	if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) != 0) {
		App::errorExit("SDL Mixer failed to open.");
	}

	/*
		Note to self:

		I don't understand why calling this before `SDL_Init` causes a segmentation fault;
		I believe it has something to do with `SDL_Quit`.
	*/
	// Register exit handler.
	std::atexit(+<::>() -> void {
		App::Performer::piano.freeAllChunks();
		App::Performer::synthesizer.freeAllChunks();

		Mix_CloseAudio();
		Mix_Quit();

		if (App::Window::window) SDL_DestroyWindow(App::Window::window);
		if (App::Window::renderer) SDL_DestroyRenderer(App::Window::renderer);

		SDL_Quit();
	});

	/* configure the sound tables with audio data */ {
		using namespace App::AudioData;

		static constexpr std::size_t byteCount{std::tuple_size_v<SimpleSound> * sizeof(SimpleSound::value_type)};

		using DataView = App::SoundTable::DataView;

		App::Performer::piano      .put(0u, DataView(Piano      ::    first.data(), byteCount));
		App::Performer::piano      .put(1u, DataView(Piano      ::    third.data(), byteCount));
		App::Performer::piano      .put(2u, DataView(Piano      ::    fifth.data(), byteCount));
		App::Performer::piano      .put(3u, DataView(Piano      ::highFirst.data(), byteCount));
		App::Performer::piano      .put(4u, DataView(Piano      ::highThird.data(), byteCount));
		App::Performer::piano      .put(5u, DataView(Piano      ::highFifth.data(), byteCount));

		App::Performer::synthesizer.put(0u, DataView(Synthesizer::    first.data(), byteCount));
		App::Performer::synthesizer.put(1u, DataView(Synthesizer::    third.data(), byteCount));
		App::Performer::synthesizer.put(2u, DataView(Synthesizer::    fifth.data(), byteCount));
		App::Performer::synthesizer.put(3u, DataView(Synthesizer::highFirst.data(), byteCount));
		App::Performer::synthesizer.put(4u, DataView(Synthesizer::highThird.data(), byteCount));
		App::Performer::synthesizer.put(5u, DataView(Synthesizer::highFifth.data(), byteCount));
	}

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

	assert(std::char_traits<char>::length(SDL_GetError()) == std::size_t{0u});
	SDL_ClearError();

	App::Window::renderer = SDL_CreateRenderer(
		App::Window::window,
		/* initialize the first renderer supporting the requested flags */ int{-1},
		#ifdef __linux__
			#ifdef __EMSCRIPTEN__
				#error The Emscripten build does not have to explicitly use a software renderer.
			#endif
			/*
				Ideally, there should be a command-line option
				to toggle between a software renderer and hardware renderer on program start.

				Prefering software renderer on Linux to avoid this error:
				"
				MESA: error: ZINK: failed to choose pdev
				glx: failed to create drisw screen
				"
			*/
			SDL_RENDERER_SOFTWARE
		#else
			SDL_RENDERER_PRESENTVSYNC
		#endif
	);

	#ifdef __linux__
	if (App::Window::renderer != nullptr and std::char_traits<char>::length(SDL_GetError()) > std::size_t{0u}) {
		#ifdef __EMSCRIPTEN__
		#error The Emscripten build may not be required to exit in this case.
		#endif
		App::errorExit(
			"There was an error with the renderer: \"", SDL_GetError(),"\"\n"
			"Consider using a software renderer instead of a hardware renderer."
		);
	}
	#endif

	if (App::Window::renderer == nullptr)
		App::Window::renderer = SDL_CreateRenderer(App::Window::window, -1, 0u);

	// Assert renderer was successfully created.
	assert(App::Window::renderer != nullptr);

	if (App::Window::renderer == nullptr) App::errorExit("Failed to create renderer for window: \"", SDL_GetError(), "\"");

	SDL_SetWindowTitle(App::Window::window, windowTitle);

	#ifndef __EMSCRIPTEN__
	SDL_SetWindowMinimumSize(App::Window::window, 250, 150);
	#endif

	#ifdef __EMSCRTIPTEN__
	App::Window::setRenderDrawColor(App::websiteBackgroundColor);
	#else
	App::Window::setRenderDrawColor(App::black);
	#endif

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
