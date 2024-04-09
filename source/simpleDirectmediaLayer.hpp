#ifndef simpleDirectmediaLayer_hpp
#define simpleDirectmediaLayer_hpp

#ifdef _WIN64
#include "SDL2/SDL.h"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL.h>
#pragma GCC diagnostic pop
#endif

#include <functional>

namespace Project::Media {
    extern Uint64 deltaTime;

    void mainLoop();

    void exitHandler();
}

#endif
