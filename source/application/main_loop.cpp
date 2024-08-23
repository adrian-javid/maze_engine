#include "main_loop.hpp"
#include "delta_time.hpp"

#include "window.hpp"

#include "performer.hpp"

namespace App {
	static decltype(getDeltaTime()) deltaTime{0u};

	decltype(getDeltaTime()) getDeltaTime() {
		return deltaTime;
	}
}


void App::mainLoop() {
	// Time of the previous iteration.
	static Uint64 previousTime{0u};

	// Get the time of this iteration.
	Uint64 const currentTime{SDL_GetTicks64()};

	// Get the change in time.
	deltaTime = currentTime - previousTime;

	for (static SDL_Event event; SDL_PollEvent(&event);) switch (event.type) {
		case SDL_KEYDOWN: switch (event.key.keysym.sym) {
			case SDLK_BACKQUOTE:
				Window::setFullscreen(true);
				break;
			case SDLK_ESCAPE:
				Window::setFullscreen(false);
				break;
		} break;
		case SDL_WINDOWEVENT: switch (event.window.event) {
			case SDL_WINDOWEVENT_RESIZED:
				Window::windowWidth = event.window.data1;
				Window::windowHeight = event.window.data2;
				break;
		} break;
		case SDL_QUIT:
			std::exit(EXIT_SUCCESS);
			break;
	}

	performer->update();

	// Refresh window.
	Window::refresh();

	// As this iteration ends, update the previous time.
	previousTime = currentTime;

	#ifndef __EMSCRIPTEN__
	// Give the CPU a break.
	SDL_Delay(1u);
	#endif
}
