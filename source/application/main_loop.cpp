#include "main_loop.hpp"
#include "delta_time.hpp"

#include "window.hpp"

#include "performer.hpp"

namespace App {
	static UnsignedMilliseconds deltaTime{0u};

	UnsignedMilliseconds getDeltaTime() {
		return deltaTime;
	}

	namespace MainLoop {
		// Time of the previous iteration.
		static UnsignedMilliseconds previousTime{0u};
	}
}

void App::mainLoop() {
	assert(performer.has_value());

	// Get the time of this iteration.
	UnsignedMilliseconds const currentTime{SDL_GetTicks64()};

	// Get the change in time.
	deltaTime = currentTime - MainLoop::previousTime;

	for (SDL_Event event; SDL_PollEvent(&event);) switch (event.type) {
		case SDL_KEYDOWN: switch (event.key.keysym.sym) {
			case SDLK_BACKQUOTE:
				Window::setFullscreen(true);
				break;
			case SDLK_ESCAPE:
				Window::setFullscreen(false);
				break;
		} break;
		case SDL_WINDOWEVENT: switch (event.window.event) {
			case /* (user initiated resize) */SDL_WINDOWEVENT_RESIZED:
			case SDL_WINDOWEVENT_SIZE_CHANGED:
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
	MainLoop::previousTime = currentTime;

	#ifndef __EMSCRIPTEN__
	// Give the CPU a break.
	SDL_Delay(1u);
	#endif
}
