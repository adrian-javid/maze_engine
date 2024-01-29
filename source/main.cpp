#include <iostream>
#include <cassert>
#include "convenience.hpp"
#include "breadth_first_search.hpp"

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
    static void renderSquareGrid(SquareGrid const &grid, std::vector<Vector2> const &path={});
}

using namespace csm4880;

static void sdl::renderSquareGrid(SquareGrid const &grid, std::vector<Vector2> const &path) {
    SDL_SetRenderDrawColor(sdl::renderer, 128, 128, 128, 255);
    SDL_RenderClear(sdl::renderer);

    int const rectangleWidth = windowWidth / safeInt(grid.getColumnCount());
    int const rectangleHeight = windowHeight / safeInt(grid.getRowCount());

    SDL_Rect rectangle{};
    rectangle.w = rectangleWidth;
    rectangle.h = rectangleHeight;
    for (size_t row{0}; row < grid.getRowCount(); ++row) {
        for (size_t column{0}; column < grid.getColumnCount(); ++column) {
            rectangle.x = safeInt(column) * rectangleWidth;
            rectangle.y = safeInt(row) * rectangleHeight;

            Uint8 green = 0x20;
            if (not grid.isWall(row, column)) green *= 5;

            SDL_SetRenderDrawColor(sdl::renderer, 0x20, green, 0x95, 0xFF);
            SDL_RenderFillRect(sdl::renderer, &rectangle);
        }
    }

    SDL_SetRenderDrawColor(sdl::renderer, 0xFF, 0x00, 0x00, 0xFF);
    for (auto const & vector : path) {
        rectangle.x = safeInt(vector.col) * rectangleWidth;
        rectangle.y = safeInt(vector.row) * rectangleHeight;
        SDL_RenderFillRect(sdl::renderer, &rectangle);
    }

    SDL_RenderPresent(sdl::renderer);
}

static void exitHandler() {
    if (sdl::window) SDL_DestroyWindow(sdl::window);

    if (sdl::renderer) SDL_DestroyRenderer(sdl::renderer);

    SDL_Quit();
}

static SquareGrid makeGrid(size_t rowCount=30, size_t columnCount=30) {
    SquareGrid grid(rowCount, columnCount);
    size_t const quarter = grid.getColumnCount() / 4;
    size_t const thirdQuarter = quarter * 3;
    for (size_t row{0}; row < grid.getRowCount(); ++row) {
        grid.putWall(row, quarter);
        grid.putWall(row, thirdQuarter);
        
        grid.putWall(row, 0);
        grid.putWall(row, grid.getColumnCount()-1);
    }

    grid.at(grid.getRowCount()-1 - 1, quarter) = SquareGrid::NONE;
    grid.at(1, thirdQuarter) = SquareGrid::NONE;

    for (size_t col{0}; col < grid.getRowCount(); ++col) {
        grid.putWall(0, col);
        grid.putWall(grid.getColumnCount()-1, col);
    }

    return grid;
}

int main(int argc, char* argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(&exitHandler);

    SquareGrid grid = makeGrid();

    auto path = breadthFirstSearch(grid, {1, 1}, {
        safeInt(grid.getRowCount())-1 - 1,
        safeInt(grid.getColumnCount())-1 - 1
    });

    SDL_CreateWindowAndRenderer(
        sdl::windowWidth, sdl::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &sdl::window, &sdl::renderer
    );

    assert(sdl::window != nullptr);
    assert(sdl::renderer != nullptr);

    sdl::renderSquareGrid(grid, *path);

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) switch (event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_WINDOWEVENT: switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    sdl::windowWidth = event.window.data1;
                    sdl::windowHeight = event.window.data2;
                    sdl::renderSquareGrid(grid, *path);
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
