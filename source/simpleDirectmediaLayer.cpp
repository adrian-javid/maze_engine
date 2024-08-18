#include "simpleDirectmediaLayer.hpp"

#include "window.hpp"

void Project::Media::exitHandler() {
	if (Media::window) SDL_DestroyWindow(Media::window);
	if (Media::renderer) SDL_DestroyRenderer(Media::renderer);
	SDL_Quit();
}
