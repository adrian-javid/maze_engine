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
    static int windowWidth = 430;
    static int windowHeight = 420;
    static SDL_Renderer *renderer = nullptr;
    static void renderSquareGrid(SquareGrid const &grid);
}

using namespace csm4880;

static inline int safeInt(size_t const value) {
    if (value <= static_cast<size_t>(std::numeric_limits<int>::max()))
        return static_cast<int>(value);
    else
        return std::numeric_limits<int>::max();
}

static void sdl::renderSquareGrid(SquareGrid const &grid) {
    SDL_SetRenderDrawColor(sdl::renderer, 128, 128, 128, 255);
    SDL_RenderClear(sdl::renderer);

    int const rectangleWidth = windowWidth / safeInt(grid.getColumnCount());
    int const rectangleHeight = windowHeight / safeInt(grid.getRowCount());

    SDL_Rect rectangle{};
    for (size_t row{0}; row < grid.getRowCount(); ++row) {
        for (size_t column{0}; column < grid.getColumnCount(); ++column) {
            rectangle.x = safeInt(column) * rectangleWidth;
            rectangle.y = safeInt(row) * rectangleHeight;
            rectangle.w = rectangleWidth;
            rectangle.h = rectangleHeight;

            Uint8 green = 0x20;
            if (grid.isWall(row, column)) green *= 5;

            SDL_SetRenderDrawColor(sdl::renderer, 0x20, green, 0x95, 0xFF);
            SDL_RenderFillRect(sdl::renderer, &rectangle);
        }
    }

    SDL_RenderPresent(sdl::renderer);
}

static void exitHandler() {
    if (sdl::window) SDL_DestroyWindow(sdl::window);

    if (sdl::renderer) SDL_DestroyRenderer(sdl::renderer);

    SDL_Quit();
}

int main(int argc, char* argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(&exitHandler);

    SDL_CreateWindowAndRenderer(
        sdl::windowWidth, sdl::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &sdl::window, &sdl::renderer
    );

    assert(sdl::window != nullptr);
    assert(sdl::renderer != nullptr);

    SquareGrid grid(10, 10);
    switch (1) {
    case 0: {
        for (size_t index{0}; index < grid.getRowCount() && index < grid.getColumnCount(); ++index)
            grid.at(index, index) = SquareGrid::NONE;
    break;}
    case 1: {
        for (size_t row{0}; row < grid.getRowCount(); ++row) {
            grid.at(row, 3) = grid.at(row, 6) = SquareGrid::WALL;
        }
        grid.at(grid.getRowCount()-1, 3) = SquareGrid::NONE;
        grid.at(0, 6) = SquareGrid::NONE;
    break;}
    }

    sdl::renderSquareGrid(grid);

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) switch (event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_WINDOWEVENT: switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    sdl::windowWidth = event.window.data1;
                    sdl::windowHeight = event.window.data2;
                    sdl::renderSquareGrid(grid);
                    break;
            } break;
            case SDL_QUIT:
                exit(EXIT_SUCCESS);
                break;
        }
        SDL_Delay(1);
    }

    return EXIT_SUCCESS;
}
