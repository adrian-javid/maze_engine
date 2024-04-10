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

namespace Project::Main {static SquareGrid generateGrid(int rowCount, int columnCount) {
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
}}

namespace Project::Main {
    static auto const maze = generateGrid(20, 20);
    static Vector2::HashSet pathTileSet;
    static double percentageWrap(double const value) { return Util::wrapValue(value, 1.00); }
}


static void refreshWindow() {
    constexpr Media::HslaColor pathTileColor(0.0);
    constexpr Media::HslaColor wallTileColor(240.0);
    constexpr Media::HslaColor emptyTileColor(155.0);

    constexpr double zeroPercent{0.0};    
    static double percentage{zeroPercent};
    double const deltaPercentage = static_cast<double>(Media::deltaTime) * 0.00010;

    percentage = Main::percentageWrap(percentage + deltaPercentage);
    assert(percentage >= 0.0); assert(percentage < 1.0);

    constexpr double depth{45.0};
    constexpr auto getColorTriplet = [](Media::HslaColor const &tileColor) -> Media::ColorTriplet {
        constexpr auto getCyclicHue = [](double const hue, double const percentageAddend) -> double {
            return Media::HslaColor::getCyclicHue(hue, Main::percentageWrap(percentage + percentageAddend), depth);
        };
        return std::make_tuple(
            tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.00)),
            tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.33)),
            tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.66))
        );
    };

    Media::ColorTriplet const pathTileColorTriplet = getColorTriplet(pathTileColor);
    Media::ColorTriplet const wallTileColorTriplet = getColorTriplet(wallTileColor);
    Media::ColorTriplet const emptyTileColorTriplet = getColorTriplet(emptyTileColor);

    Media::drawRectangleGrid(
        {0.0f, 0.0f},
        Main::maze.RowCount(), Main::maze.ColumnCount(),
        static_cast<float>(Media::windowWidth),
        static_cast<float>(Media::windowHeight),
        [
            &pathTileColorTriplet, &wallTileColorTriplet, &emptyTileColorTriplet
        ](int row, int column) -> Media::ColorTriplet {
            if (Main::pathTileSet.find({row, column}) != Main::pathTileSet.end())
                return pathTileColorTriplet;
            else if (Main::maze.isWall(row, column))
                return wallTileColorTriplet;
            else
                return emptyTileColorTriplet;
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
