#include <iostream>

#ifdef _WIN64
#include "SDL2/SDL.h"
#elif defined(__linux__)
#include <SDL.h>
#else
static_assert(false, "Unsupported platform.");
#endif

#include "SquareGrid.hpp"

int main(int argc, char* argv[]) {
    using namespace csm4880;

    SDL_Init(0);

    std::cout << "Begin.\n";

    SquareGrid<3, 10> grid;

    std::cout << "Size: " << grid.getSize() << '\n';

    grid.at(0, 0) = true;
    grid.at(1, 1) = true;
    grid.at(2, 2) = true;

    std::cout << grid.flatData << '\n';
    std::cout << grid << '\n';
    std::cout << "Done.\n";

    SDL_Quit();
    return EXIT_SUCCESS;
}
