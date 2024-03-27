#include <cassert>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "breadthFirstSearch.hpp"
#include "Cast.hpp"
#include "graphics.hpp"
#include "SquareGrid.hpp"

using namespace Project;

static SquareGrid makeGrid(CommonInt rowCount, CommonInt columnCount) {
    SquareGrid grid(rowCount, columnCount);
    CommonInt const secondQuarter = grid.ColumnCount() / 4;
    CommonInt const fourthQuarter = secondQuarter * 3;
    for (CommonInt row{0}; row < grid.RowCount(); ++row) {
        grid.putWall(row, secondQuarter);
        grid.putWall(row, fourthQuarter);
        
        grid.putWall(row, 0);
        grid.putWall(row, grid.ColumnCount() - 1);
    }

    grid.at((grid.RowCount() - 1) - 1, secondQuarter) = SquareGrid::emptyTile;
    grid.at(1, fourthQuarter) = SquareGrid::emptyTile;

    for (CommonInt offset = 0; offset < 8; ++offset) {
        grid.putWall(16, secondQuarter + offset);
        grid.putWall(3, fourthQuarter - offset);
    }

    grid.at((grid.RowCount() - 1) - 2, secondQuarter) = SquareGrid::emptyTile;
    grid.at(2, fourthQuarter) = SquareGrid::emptyTile;

    for (CommonInt col{0}; col < grid.RowCount(); ++col) {
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
    std::atexit(&Sdl::exitHandler);

    #ifndef NDEBUG
    Sdl::HslaColor color{210, 0.79, 0.3, 0.5};
    O << color.toString() << "\n";
    // O << color.toRgbaColor().toString() << "\n";
    // O << Sdl::HslaColor{240.0, 1.0, 0.5, 1.0}.toRgbaColor().toString() << "\n";
    // O << Sdl::HslaColor{120.0, 1.0, 0.5, 1.0}.toRgbaColor().toString() << "\n";
    // O << Sdl::HslaColor{60.0, 1.0, 0.5, 1.0}.toRgbaColor().toString() << "\n";
    #endif

    Sdl::globalGrid = makeGrid(20, 20);

    int const lastRow = Sdl::globalGrid.RowCount() - 1;
    int const lastColumn = Sdl::globalGrid.ColumnCount() - 1;
    auto const path = breadthFirstSearch(Sdl::globalGrid, {0 + 1, 0 + 1}, {lastRow - 1, lastColumn - 1});

    for (auto &vector : path.value()) {
        Sdl::globalColorMap.insert({vector, Sdl::PATH_COLOR});
    }

    SDL_CreateWindowAndRenderer(
        Sdl::windowWidth, Sdl::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &Sdl::window, &Sdl::renderer
    );

    assert(Sdl::window != nullptr);
    assert(Sdl::renderer != nullptr);

    SDL_SetWindowTitle(Sdl::window, "Maze Solver");
    SDL_SetWindowMinimumSize(Sdl::window, 250, 150);

    Sdl::refreshPresentation();

    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&Sdl::mainLoop, -1, true);
    #else
    while (true) Sdl::mainLoop();
    #endif

    return EXIT_SUCCESS;
}
