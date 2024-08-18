#include "main.hpp"

#include "window.hpp"

void Project::Main::exitHandler() {
	if (Media::window) SDL_DestroyWindow(Media::window);
	if (Media::renderer) SDL_DestroyRenderer(Media::renderer);
	SDL_Quit();
}