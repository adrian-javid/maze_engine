#ifndef graphics_hpp
#define graphics_hpp

#ifdef _WIN64
#include "SDL2/SDL.h"
#else
#include <SDL.h>
#endif

#include "SquareGrid.hpp"
#include "Vector2.hpp"
#include "cast.hpp"

namespace Project::Sdl {

    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern int windowWidth;
    extern int windowHeight;
    extern SDL_Event event;

    struct Color {
        Uint8 red, green, blue, alpha;
        inline void SetRenderDrawColor() const;
        constexpr Color withRed  (Uint8 const red)   const { return {red, green, blue, alpha}; }
        constexpr Color withGreen(Uint8 const green) const { return {red, green, blue, alpha}; }
        constexpr Color withBlue (Uint8 const blue)  const { return {red, green, blue, alpha}; }
        constexpr Color withAlpha(Uint8 const alpha) const { return {red, green, blue, alpha}; }
    };

    constexpr Color BLACK{0x00, 0x00, 0x00, 0xFF};
    constexpr Color PATH_COLOR = BLACK.withRed(0xFF);

    extern SquareGrid grid;
    extern Vector2::HashMap<Sdl::Color> colorMap;

    void renderSquareGrid(SquareGrid const &grid=Sdl::grid, Vector2::HashMap<Color> const &colorMap=Sdl::colorMap);

    void drawHexagon(SDL_FPoint const &center, float const size);
    void drawHexagon(SDL_FPoint const &center, float const width, float const height);

    void mainLoop();
    void exitHandler();
}

#endif
