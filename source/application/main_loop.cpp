#include "application/common.hpp"

#include "application/window.hpp"

#include "application/performer.hpp"

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

	// Give the CPU a break.
	SDL_Delay(1u);
}
