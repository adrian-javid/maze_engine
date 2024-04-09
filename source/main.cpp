#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "breadthFirstSearch.hpp"
#include "simpleDirectmediaLayer.hpp"
#include "window.hpp"
#include "SquareGrid.hpp"
#include "Util.hpp"

#include <algorithm>
#include <iostream>

#if true
static auto &O = std::cout;
#endif

using namespace Project;

static SquareGrid generateGrid(int rowCount, int columnCount) {
    SquareGrid grid(rowCount, columnCount);
    int const secondQuarter = grid.ColumnCount() / 4;
    int const fourthQuarter = secondQuarter * 3;
    for (int row{0}; row < grid.RowCount(); ++row) {
        grid.putWall(row, secondQuarter);
        grid.putWall(row, fourthQuarter);
        
        grid.putWall(row, 0);
        grid.putWall(row, grid.ColumnCount() - 1);
    }

    grid.at((grid.RowCount() - 1) - 1, secondQuarter) = SquareGrid::emptyTile;
    grid.at(1, fourthQuarter) = SquareGrid::emptyTile;

    for (int offset = 0; offset < 8; ++offset) {
        grid.putWall(16, secondQuarter + offset);
        grid.putWall(3, fourthQuarter - offset);
    }

    grid.at((grid.RowCount() - 1) - 2, secondQuarter) = SquareGrid::emptyTile;
    grid.at(2, fourthQuarter) = SquareGrid::emptyTile;

    for (int col{0}; col < grid.RowCount(); ++col) {
        grid.putWall(0, col);
        grid.putWall(grid.ColumnCount() - 1, col);
    }

    return grid;
}

namespace Project::Main {
    static auto const maze = generateGrid(20, 20);
    static Vector2::HashSet pathTileSet;
}

static void refreshWindow() {
    constexpr Media::HslaColor wallTileColor(225.0);
    constexpr Media::HslaColor emptyTileColor(175.0);
    constexpr Media::HslaColor pathTileColor(0.0);

    static double percentage{0.0};

    double const deltaPercentage = static_cast<double>(Media::deltaTime) / 32.0;

    percentage = Util::wrapValue(percentage + deltaPercentage, 1.0);
    assert(percentage >= 0.0);
    assert(percentage < 1.0);

    static Uint64 timer = 0;
    static Uint64 constexpr oneSecond = 1'000;
    if ((timer += Media::deltaTime) >= oneSecond / 6) {
        O << percentage << '\n';
        timer = 0; // reset timer
    }

    Media::setRenderDrawColor(Media::black);
    SDL_RenderClear(Media::renderer);

    Media::drawRectangleGrid(
        {0.0f, 0.0f},
        Main::maze.RowCount(), Main::maze.ColumnCount(),
        static_cast<float>(Media::windowWidth),
        static_cast<float>(Media::windowHeight),
        [&](int row, int column) -> Media::ColorTriplet {

            SDL_Color const color = [&]() -> SDL_Color {
                if (Main::pathTileSet.count({row, column}))
                    return pathTileColor.toRgbaColor();
                else if (Main::maze.isWall(row, column))
                    return wallTileColor.toRgbaColor();
                else
                    return emptyTileColor.toRgbaColor();
            }();

            return {color, color, color};
        }
    );

    SDL_RenderPresent(Media::renderer);
}

int main(int argc, char *argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    SDL_Init(SDL_INIT_VIDEO);

    /*
        I want to investigate why
        calling this before `SDL_Init` causes a segmentation fault.

        I believe it has something to do with `SDL_Quit`.
    */
    std::atexit(&Media::exitHandler);

    int const lastRow = Main::maze.RowCount() - 1;
    int const lastColumn = Main::maze.ColumnCount() - 1;
    Vector2 const start = {0 + 1, 0 + 1};
    Vector2 const end = {lastRow - 1, lastColumn - 1};
    auto const path = breadthFirstSearch(Main::maze, start, end);
    for (auto &vector : path.value()) Main::pathTileSet.insert(vector);

    SDL_CreateWindowAndRenderer(
        Media::windowWidth, Media::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &Media::window, &Media::renderer
    );

    assert(Media::window != nullptr);
    assert(Media::renderer != nullptr);

    SDL_SetWindowTitle(Media::window, "Maze Solver");
    SDL_SetWindowMinimumSize(Media::window, 250, 150);

    Media::windowRefresher = &refreshWindow;
    Media::windowRefresher();

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&Media::mainLoop, -1, true);
    #else
    while (true) Media::mainLoop();
    #endif

    return EXIT_SUCCESS;
}
