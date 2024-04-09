#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "breadthFirstSearch.hpp"
#include "graphics.hpp"
#include "SquareGrid.hpp"

using namespace Project;

static SquareGrid makeGrid(int rowCount, int columnCount) {
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

#ifndef NDEBUG
#include <iostream>
#endif

int main(int argc, char *argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    #ifndef NDEBUG
    static auto &O = std::cout;
    #endif

    SDL_Init(SDL_INIT_VIDEO);
    std::atexit(&Media::exitHandler);

    #ifndef NDEBUG
    Media::HslaColor color{210, 0.79, 0.3, 0.5};
    O << color.toString() << "\n";
    // O << color.toRgbaColor().toString() << "\n";
    // O << Media::HslaColor{240.0, 1.0, 0.5, 1.0}.toRgbaColor().toString() << "\n";
    // O << Media::HslaColor{120.0, 1.0, 0.5, 1.0}.toRgbaColor().toString() << "\n";
    // O << Media::HslaColor{60.0, 1.0, 0.5, 1.0}.toRgbaColor().toString() << "\n";
    #endif

    Media::globalGrid = makeGrid(20, 20);

    int const lastRow = Media::globalGrid.RowCount() - 1;
    int const lastColumn = Media::globalGrid.ColumnCount() - 1;
    auto const path = breadthFirstSearch(Media::globalGrid, {0 + 1, 0 + 1}, {lastRow - 1, lastColumn - 1});

    for (auto &vector : path.value()) {
        Media::globalColorMap.insert({vector, Media::PATH_COLOR});
    }

    SDL_CreateWindowAndRenderer(
        Media::windowWidth, Media::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &Media::window, &Media::renderer
    );

    assert(Media::window != nullptr);
    assert(Media::renderer != nullptr);

    SDL_SetWindowTitle(Media::window, "Maze Solver");
    SDL_SetWindowMinimumSize(Media::window, 250, 150);

    Media::refreshPresentation();

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&Media::mainLoop, -1, true);
    #else
    while (true) Media::mainLoop();
    #endif

    return EXIT_SUCCESS;
}
