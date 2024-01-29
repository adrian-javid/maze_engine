#include <iostream>
#include <cassert>
#include <limits>

#ifdef _WIN64
#include "SDL2/SDL.h"
#elif defined(__linux__)
#include <SDL.h>
#else
static_assert(false, "Unsupported platform.");
#endif

#include "SquareGrid.hpp"

namespace csm4880::sdl {
    static SDL_Window *window = nullptr;
    static SDL_Renderer *renderer = nullptr;
}

using namespace csm4880;
    
static void exitHandler() {
    if (sdl::window) SDL_DestroyWindow(sdl::window);

    if (sdl::renderer) SDL_DestroyRenderer(sdl::renderer);

    SDL_Quit();
}

static inline int safeInt(size_t const value) {
    if (value <= static_cast<size_t>(std::numeric_limits<int>::max()))
        return static_cast<int>(value);
    else
        return std::numeric_limits<int>::max();
}

int main(int argc, char* argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(&exitHandler);

    SDL_CreateWindowAndRenderer(430, 420, 0, &sdl::window, &sdl::renderer);
    assert(sdl::window != nullptr); assert(sdl::renderer != nullptr);
    SDL_SetRenderDrawColor(sdl::renderer, 128, 128, 128, 255); SDL_RenderClear(sdl::renderer);

    SquareGrid grid(10, 10);
    for (size_t index{0}; index < grid.getRowCount() && index < grid.getColumnCount(); ++index)
        grid.at(index, index) = true;

    int constexpr rectangleWidth = 30; int constexpr rectangleHeight = 30;

    SDL_Rect rectangle{};
    for (size_t row{0}; row < grid.getRowCount(); ++row) {
        for (size_t column{0}; column < grid.getColumnCount(); ++column) {
            rectangle.x = safeInt(column) * rectangleWidth;
            rectangle.y = safeInt(row) * rectangleHeight;
            rectangle.w = rectangleWidth;
            rectangle.h = rectangleHeight;

            Uint8 green = 0x20;
            switch (grid.at(row, column)) { case SquareGrid::WALL: green *= 5; }

            SDL_SetRenderDrawColor(sdl::renderer, 0x20, green, 0x95, 0xFF);
            SDL_RenderFillRect(sdl::renderer, &rectangle);
        }
    }

    SDL_RenderPresent(sdl::renderer);

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event))
            switch (event.type) { case SDL_QUIT: case SDL_KEYDOWN: exit(EXIT_SUCCESS); }
        SDL_Delay(1);
    }

    return EXIT_SUCCESS;
}
