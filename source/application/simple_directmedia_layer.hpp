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
#include "SDL.h"
#pragma GCC diagnostic pop
#endif

#endif
