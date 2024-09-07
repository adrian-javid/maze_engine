#include "window.hpp"

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
EMSCRIPTEN_BINDINGS(Window) {
	emscripten::function("Window_setFullscreen", &App::Window::setFullscreen);
	emscripten::function("Window_setSize", +[](int const width, int const height) {
		assert(App::Window::window != nullptr);
		SDL_SetWindowSize(App::Window::window, width, height);
	});
}
#endif

namespace App::Window {
	SDL_Window *window{nullptr};
	SDL_Renderer *renderer{nullptr};
	int windowWidth{450};
	int windowHeight{450};
}

