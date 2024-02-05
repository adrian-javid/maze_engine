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

    struct RgbaColor {
        Uint8 red, green, blue, alpha;
        void SetRenderDrawColor() const;
        constexpr RgbaColor withRed  (Uint8 const red)   const { return {red, green, blue, alpha}; }
        constexpr RgbaColor withGreen(Uint8 const green) const { return {red, green, blue, alpha}; }
        constexpr RgbaColor withBlue (Uint8 const blue)  const { return {red, green, blue, alpha}; }
        constexpr RgbaColor withAlpha(Uint8 const alpha) const { return {red, green, blue, alpha}; }
        std::string toString() const;
    };

    struct HslaColor {
        double hue, saturation, luminance, alpha;
        RgbaColor toRgbaColor() const;
        std::string toString() const;
    };

    constexpr RgbaColor BLACK{0x00, 0x00, 0x00, 0xFF};
    constexpr RgbaColor PATH_COLOR = BLACK.withRed(0xFF);

    extern SquareGrid grid;
    extern Vector2::HashMap<Sdl::RgbaColor> colorMap;

    void refreshPresentation();

    void renderSquareGrid(SquareGrid const &grid=Sdl::grid, Vector2::HashMap<RgbaColor> const &colorMap=Sdl::colorMap);

    void drawPointyTopHexagon(float const size, SDL_FPoint const &center);
    void drawPointyTopHexagon(SDL_FPoint const &center, float const width, float const height);

    void mainLoop();
    void exitHandler();
}

#endif
