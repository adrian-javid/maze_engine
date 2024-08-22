#include "application/window.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(Window) {
	emscripten::function("Window_setFullscreen", &App::Window::setFullscreen);
}
#endif

namespace App::Window {
	SDL_Window *window = nullptr;
	SDL_Renderer *renderer = nullptr;
	int windowWidth = 420;
	int windowHeight = 420;
}

