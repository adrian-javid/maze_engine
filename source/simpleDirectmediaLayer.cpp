#include "simpleDirectmediaLayer.hpp"

#include "window.hpp"

// Change in time.
Uint64 Project::Media::deltaTime = 0;

void Project::Media::mainLoop() {
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
                SDL_SetWindowFullscreen(Media::window, SDL_WINDOW_FULLSCREEN);
                break;
            case SDLK_ESCAPE:
                SDL_SetWindowFullscreen(Media::window, 0u);
                break;
        } break;
        case SDL_WINDOWEVENT: switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                Media::windowWidth = event.window.data1;
                Media::windowHeight = event.window.data2;
                break;
        } break;
        case SDL_QUIT:
            std::exit(EXIT_SUCCESS);
            break;
    }

    // Refresh window.
    Media::windowRefresher();

    // As this iteration ends, update the previous time.
    previousTime = currentTime;

    // Give the CPU a break.
    SDL_Delay(1u);
}

void Project::Media::exitHandler() {
    if (Media::window) SDL_DestroyWindow(Media::window);
    if (Media::renderer) SDL_DestroyRenderer(Media::renderer);
    SDL_Quit();
}
