#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "breadthFirstSearch.hpp"
#include "Media.hpp"
#include "SquareGrid.hpp"

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

#ifndef NDEBUG
#include <iostream>
#endif

int main(int argc, char *argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    std::atexit(&Media::exitHandler);
    SDL_Init(SDL_INIT_VIDEO);

    Grid const &maze = generateGrid(20, 20);

    int const lastRow = Media::globalGrid.RowCount() - 1;
    int const lastColumn = Media::globalGrid.ColumnCount() - 1;
    auto const path = breadthFirstSearch(Media::globalGrid, {0 + 1, 0 + 1}, {lastRow - 1, lastColumn - 1});

    for (auto &vector : path.value()) {
        Media::globalColorMap.insert({vector, Media::pathColor});
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

    Media::refreshWindow_v0();

    Media::windowRefresher = []() -> void {};

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&Media::mainLoop, -1, true);
    #else
    while (true) Media::mainLoop();
    #endif

    return EXIT_SUCCESS;
}
