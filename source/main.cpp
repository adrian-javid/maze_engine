#include <iostream>
#include <cassert>
#include "cast.hpp"
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
    static SDL_Renderer *renderer = nullptr;
    static int windowWidth = 420;
    static int windowHeight = 420;
    
    struct Color {
        Uint8 red, green, blue, alpha;
        inline void SetRenderDrawColor() const { SDL_SetRenderDrawColor(sdl::renderer, red, green, blue, alpha); }
        inline constexpr Color withRed  (Uint8 const red)   const { return {red, green, blue, alpha}; }
        inline constexpr Color withGreen(Uint8 const green) const { return {red, green, blue, alpha}; }
        inline constexpr Color withBlue (Uint8 const blue)  const { return {red, green, blue, alpha}; }
        inline constexpr Color withAlpha(Uint8 const alpha) const { return {red, green, blue, alpha}; }
    };

    constexpr Color PATH_COLOR{0xFF, 0x00, 0x00, 0xFF};

    static void renderSquareGrid(SquareGrid const &grid, Vector2::HashMap<Color> const &colorMap);

    static void exitHandler() {
        if (sdl::window) SDL_DestroyWindow(sdl::window);
        if (sdl::renderer) SDL_DestroyRenderer(sdl::renderer);
        SDL_Quit();
    }

}

using namespace csm4880;

static void sdl::renderSquareGrid(SquareGrid const &grid, Vector2::HashMap<Color> const &colorMap) {
    static constexpr sdl::Color wallColor{0x20, 0x20, 0x95, 0xFF};
    static constexpr sdl::Color defaultColor = wallColor.withGreen(wallColor.green * 5);

    wallColor.SetRenderDrawColor();
    SDL_RenderClear(sdl::renderer);

    int const rectangleWidth = windowWidth / cast::toInt(grid.getColumnCount());
    int const rectangleHeight = windowHeight / cast::toInt(grid.getRowCount());

    SDL_Rect rectangle{};
    rectangle.w = rectangleWidth;
    rectangle.h = rectangleHeight;

    int rowCount = cast::toInt(grid.getRowCount());
    int columnCount = cast::toInt(grid.getColumnCount());

    for (Vector2 vector(0, 0); vector.row < rowCount; ++vector.row) {
        for (vector.col = 0; vector.col < columnCount; ++vector.col) {
            rectangle.x = vector.col * rectangleWidth;
            rectangle.y = vector.row * rectangleHeight;
            
            if (colorMap.count(vector))
                colorMap.at(vector).SetRenderDrawColor();
            else if (grid.isWall(vector.row, vector.col))
                wallColor.SetRenderDrawColor();
            else
                defaultColor.SetRenderDrawColor();

            SDL_RenderFillRect(sdl::renderer, &rectangle);
        }
    }

    SDL_RenderPresent(sdl::renderer);
}

static SquareGrid makeGrid(size_t rowCount=20, size_t columnCount=20) {
    SquareGrid grid(rowCount, columnCount);
    size_t const quarter = grid.getColumnCount() / 4;
    size_t const thirdQuarter = quarter * 3;
    for (size_t row{0}; row < grid.getRowCount(); ++row) {
        grid.putWall(row, quarter);
        grid.putWall(row, thirdQuarter);
        
        grid.putWall(row, 0);
        grid.putWall(row, grid.getColumnCount() - 1);
    }

    grid.at((grid.getRowCount() - 1) - 1, quarter) = SquareGrid::NONE;
    grid.at(1, thirdQuarter) = SquareGrid::NONE;

    for (size_t col{0}; col < grid.getRowCount(); ++col) {
        grid.putWall(0, col);
        grid.putWall(grid.getColumnCount() - 1, col);
    }

    return grid;
}

int main(int argc, char* argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(&sdl::exitHandler);

    SquareGrid grid = makeGrid();

    int const lastRow = cast::toInt(grid.getRowCount()) - 1;
    int const lastColumn = cast::toInt(grid.getColumnCount()) - 1;
    auto const path = breadthFirstSearch(grid, {0 + 1, 0 + 1}, {lastRow - 1, lastColumn - 1});

    Vector2::HashMap<sdl::Color> colorMap;
    for (auto &vector : path.value()) {
        colorMap.insert({vector, sdl::PATH_COLOR});
    }

    SDL_CreateWindowAndRenderer(
        sdl::windowWidth, sdl::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &sdl::window, &sdl::renderer
    );

    assert(sdl::window != nullptr);
    assert(sdl::renderer != nullptr);

    sdl::renderSquareGrid(grid, colorMap);

    SDL_Event event;
    while (true) {
        while (SDL_PollEvent(&event)) switch (event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_WINDOWEVENT: switch (event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    sdl::windowWidth = event.window.data1;
                    sdl::windowHeight = event.window.data2;
                    sdl::renderSquareGrid(grid, colorMap);
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
