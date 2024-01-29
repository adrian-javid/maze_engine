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
    static_cast<void>(argc);
    static_cast<void>(argv);

    using namespace csm4880;

    SDL_Init(0);

    std::cout << "Begin.\n";

    SquareGrid grid(10, 10);

    for (size_t index{0}; index < grid.getRowCount() && index < grid.getColumnCount(); ++index) {
        std::cout << index << " | " << static_cast<unsigned int>(grid.at(index, index)) << '\n';
        grid.at(index, index) = true;
    }

    for (auto &tile : grid.getFlatData()) {
        std::cout << static_cast<unsigned short>(tile);
    }
    std::cout << '\n';

    std::cout << grid << '\n';
    std::cout << "Done.\n";

    SDL_Quit();
    return EXIT_SUCCESS;
}
