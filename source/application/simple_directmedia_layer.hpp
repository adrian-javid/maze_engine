#ifndef Application_SimpleDirectmediaLayer_hpp
#define Application_SimpleDirectmediaLayer_hpp

#ifdef _WIN64
#pragma warning(push, 0)
#include "SDL2/SDL.h"
#pragma warning(pop)
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wall"
#pragma GCC diagnostic ignored "-Wextra"
#pragma GCC diagnostic ignored "-Wpedantic"
#pragma GCC diagnostic ignored "-Wold-style-cast"
#pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#include "SDL.h"
#include "SDL_mixer.h"
#pragma GCC diagnostic pop
#endif

#endif
