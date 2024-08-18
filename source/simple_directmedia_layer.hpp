#ifndef SIMPLE_DIRECTMEDIA_LATER_HPP
#define SIMPLE_DIRECTMEDIA_LATER_HPP

#ifdef _WIN64
#include "SDL2/SDL.h"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include "SDL.h"
#pragma GCC diagnostic pop
#endif

#endif
