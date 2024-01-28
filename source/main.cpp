
#include <iostream>

#ifdef _WIN64
#include "SDL2/SDL.h"
#else
static_assert(false)
#endif

int main(int argc, char* argv[]) {
    SDL_Init(0);
    std::cout << "Begin.\n";
    return EXIT_SUCCESS;
    SDL_Quit();
}
