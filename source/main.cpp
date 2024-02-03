#include <iostream>
#include <cassert>
#include "cast.hpp"
#include "breadth_first_search.hpp"

#ifdef _WIN64
#include "SDL2/SDL.h"
#else
#include <SDL.h>
#endif

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "SquareGrid.hpp"

namespace Project::Sdl {

    static SDL_Window *window = nullptr;
    static SDL_Renderer *renderer = nullptr;
    static int windowWidth = 420;
    static int windowHeight = 420;
    static SDL_Event event;

    struct Color {
        Uint8 red, green, blue, alpha;
        inline void SetRenderDrawColor() const { SDL_SetRenderDrawColor(Sdl::renderer, red, green, blue, alpha); }
        inline constexpr Color withRed  (Uint8 const red)   const { return {red, green, blue, alpha}; }
        inline constexpr Color withGreen(Uint8 const green) const { return {red, green, blue, alpha}; }
        inline constexpr Color withBlue (Uint8 const blue)  const { return {red, green, blue, alpha}; }
        inline constexpr Color withAlpha(Uint8 const alpha) const { return {red, green, blue, alpha}; }
    };

    constexpr Color BLACK{0x00, 0x00, 0x00, 0xFF};
    constexpr Color PATH_COLOR = BLACK.withRed(0xFF);

    static SquareGrid grid;
    static Vector2::HashMap<Sdl::Color> colorMap;
    static void renderSquareGrid(SquareGrid const &grid=Sdl::grid, Vector2::HashMap<Color> const &colorMap=Sdl::colorMap);

    static void tick() {
        while (SDL_PollEvent(&Sdl::event)) switch (Sdl::event.type) {
            case SDL_KEYDOWN:
                break;
            case SDL_WINDOWEVENT: switch (Sdl::event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    Sdl::windowWidth = Sdl::event.window.data1;
                    Sdl::windowHeight = Sdl::event.window.data2;
                    Sdl::renderSquareGrid();
                    break;
            } break;
            case SDL_QUIT:
                std::exit(EXIT_SUCCESS);
                break;
        }
        SDL_Delay(1);
    }

    static void exitHandler() {
        if (Sdl::window) SDL_DestroyWindow(Sdl::window);
        if (Sdl::renderer) SDL_DestroyRenderer(Sdl::renderer);
        SDL_Quit();
    }

}

using namespace Project;

static void Sdl::renderSquareGrid(SquareGrid const &grid, Vector2::HashMap<Color> const &colorMap) {
    static constexpr Sdl::Color wallColor{0x20, 0x20, 0x95, 0xFF};
    static constexpr Sdl::Color defaultColor = wallColor.withGreen(wallColor.green * 5);

    Sdl::BLACK.SetRenderDrawColor();
    SDL_RenderClear(Sdl::renderer);

    int const rectangleWidth = windowWidth / Cast::toInt(grid.getColumnCount());
    int const rectangleHeight = windowHeight / Cast::toInt(grid.getRowCount());

    SDL_Rect rectangle{};
    rectangle.w = rectangleWidth;
    rectangle.h = rectangleHeight;

    int rowCount = Cast::toInt(grid.getRowCount());
    int columnCount = Cast::toInt(grid.getColumnCount());

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

            SDL_RenderFillRect(Sdl::renderer, &rectangle);
        }
    }

    SDL_RenderPresent(Sdl::renderer);
}

static SquareGrid makeGrid(size_t rowCount=20, size_t columnCount=20) {
    SquareGrid grid(rowCount, columnCount);
    size_t const secondQuarter = grid.getColumnCount() / 4;
    size_t const fourthQuarter = secondQuarter * 3;
    for (size_t row{0}; row < grid.getRowCount(); ++row) {
        grid.putWall(row, secondQuarter);
        grid.putWall(row, fourthQuarter);
        
        grid.putWall(row, 0);
        grid.putWall(row, grid.getColumnCount() - 1);
    }

    grid.at((grid.getRowCount() - 1) - 1, secondQuarter) = SquareGrid::NONE;
    grid.at(1, fourthQuarter) = SquareGrid::NONE;

    for (size_t offset = 0; offset < 8; ++offset) {
        grid.putWall(16, secondQuarter + offset);
        grid.putWall(3, fourthQuarter - offset);

    }

    grid.at((grid.getRowCount() - 1) - 2, secondQuarter) = SquareGrid::NONE;
    grid.at(2, fourthQuarter) = SquareGrid::NONE;

    for (size_t col{0}; col < grid.getRowCount(); ++col) {
        grid.putWall(0, col);
        grid.putWall(grid.getColumnCount() - 1, col);
    }

    return grid;
}

int main(int argc, char *argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(&Sdl::exitHandler);

    Sdl::grid = makeGrid();

    int const lastRow = Cast::toInt(Sdl::grid.getRowCount()) - 1;
    int const lastColumn = Cast::toInt(Sdl::grid.getColumnCount()) - 1;
    auto const path = breadthFirstSearch(Sdl::grid, {0 + 1, 0 + 1}, {lastRow - 1, lastColumn - 1});

    for (auto &vector : path.value()) {
        Sdl::colorMap.insert({vector, Sdl::PATH_COLOR});
    }

    SDL_CreateWindowAndRenderer(
        Sdl::windowWidth, Sdl::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &Sdl::window, &Sdl::renderer
    );

    assert(Sdl::window != nullptr);
    assert(Sdl::renderer != nullptr);

    SDL_SetWindowTitle(Sdl::window, "Maze Solver");

    Sdl::renderSquareGrid(Sdl::grid, Sdl::colorMap);

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&Sdl::tick, -1, true);
    #else
    while (true) Sdl::tick();
    #endif

    return EXIT_SUCCESS;
}
