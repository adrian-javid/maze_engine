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


static double percentageWrap(double const value) { return Util::wrapValue(value, 1.00); }

static void refreshWindow() {
    constexpr Media::HslaColor tileColor(225.0);

    static double percentage{0.0};

    double const deltaPercentage = static_cast<double>(Media::deltaTime) * 0.00010;

    percentage = percentageWrap(percentage + deltaPercentage);
    assert(percentage >= 0.0);
    assert(percentage < 1.0);

    double constexpr colorDepth{55.0};

    static Uint64 timer{0};
    static Uint64 constexpr oneSecond{1'000};
    constexpr double depth{50.0};

    auto const hueOffset = Util::linearInterpolation(percentage, 0.0, 2.0 * depth);

    auto const cyclicHue0 = Media::HslaColor::getCyclicHue(tileColor.hue, percentage, depth);
    auto const rgbaColor0 = tileColor.toRgbaColor(cyclicHue0);

    auto const cyclicHue1 = Media::HslaColor::getCyclicHue(tileColor.hue, percentageWrap(percentage - .10), depth);
    auto const rgbaColor1 = tileColor.toRgbaColor(cyclicHue1);

    auto const cyclicHue2 = Media::HslaColor::getCyclicHue(tileColor.hue, percentageWrap(percentage - .20), depth);
    auto const rgbaColor2 = tileColor.toRgbaColor(cyclicHue2);

    auto constexpr sp = ' ';
    auto constexpr ln = '\n';

    if ((timer += Media::deltaTime) >= oneSecond / 10) {
        O
            << percentage << sp
            << hueOffset << sp
            << cyclicHue0 << sp
            << cyclicHue1 << sp
            << cyclicHue2 << sp
        << ln;
        timer = 0; // reset timer
    }

    Media::drawRectangleGrid(
        {0.0f, 0.0f},
        Main::maze.RowCount(), Main::maze.ColumnCount(),
        static_cast<float>(Media::windowWidth),
        static_cast<float>(Media::windowHeight),
        [&](int row, int column) -> Media::ColorTriplet {
            return {rgbaColor0, rgbaColor1, rgbaColor2};
        }
    );

    SDL_RenderPresent(Media::renderer);
}

int main(int argc, char *argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    // Initialize the Simple Directmedia Layer library.
    SDL_Init(SDL_INIT_VIDEO);

    // Register exit handler.
    /*
        I don't understand why
        calling this before `SDL_Init` causes a segmentation fault.

        I believe it has something to do with `SDL_Quit`.
    */
    std::atexit(&Media::exitHandler);

    // Get the last row index and the last column index of the maze.
    int const lastRow = Main::maze.RowCount() - 1;
    int const lastColumn = Main::maze.ColumnCount() - 1;

    // Define start and end positions of the maze.
    Vector2 const start = {0 + 1, 0 + 1};
    Vector2 const end = {lastRow - 1, lastColumn - 1};

    // Search for a path that solves the maze.
    auto const path = breadthFirstSearch(Main::maze, start, end);

    // Save the path tiles.
    for (auto &vector : path.value()) Main::pathTileSet.insert(vector);

    // You couldn't have guessed that this creates the window and renderer.
    SDL_CreateWindowAndRenderer(
        Media::windowWidth, Media::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &Media::window, &Media::renderer
    );

    // Assert window and renderer were created.
    assert(Media::window != nullptr);
    assert(Media::renderer != nullptr);

    SDL_SetWindowTitle(Media::window, "Maze Solver");
    SDL_SetWindowMinimumSize(Media::window, 250, 150);

    // Set the window refresher. This is called every iteration in the main loop.
    Media::windowRefresher = &refreshWindow;
    // Media::windowRefresher = &Media::refreshWindow_v0;

    // Start the main loop.
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&Media::mainLoop, -1, true);
    #else
    while (true) Media::mainLoop();
    #endif

    return EXIT_SUCCESS;
}
