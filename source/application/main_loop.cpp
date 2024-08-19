#include "application/common.hpp"

#include "application/window.hpp"

/*
	There are several state the program can be in.

	1. Idle
		In this state, the program is waiting until the user prompts
		for the program to begin solving the maze.
	2. Performing
		In this state, the program is generating or solving a maze.
*/

#if false
namespace App {
	static void idle() {}
	static void perform() {}
	static void (*actionState)() = perform;
	static void doIdle() { actionState = idle; }
	static void doPerform() { actionState = perform; }
}
#endif

void App::mainLoop() {
	// Time of the previous iteration.
	static Uint64 previousTime{0u};

	// Get the time of this iteration.
	Uint64 const currentTime = SDL_GetTicks64();

	// Get the change in time.
	deltaTime = currentTime - previousTime;

	static SDL_Event event;
	while (SDL_PollEvent(&event)) switch (event.type) {
		case SDL_KEYDOWN: switch (event.key.keysym.sym) {
			case SDLK_BACKQUOTE:
				SDL_SetWindowFullscreen(Window::window, SDL_WINDOW_FULLSCREEN);
				break;
			case SDLK_ESCAPE:
				SDL_SetWindowFullscreen(Window::window, 0u);
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

	// Refresh window.
	Window::refresh();

	// As this iteration ends, update the previous time.
	previousTime = currentTime;

	// Give the CPU a break.
	SDL_Delay(1u);
}
