#ifndef graphics_hpp
#define graphics_hpp

#ifdef _WIN64
#include "SDL2/SDL.h"
#else
#include <SDL.h>
#endif

#include "SquareGrid.hpp"
#include "Vector2.hpp"

namespace Project::Sdl {

    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern int windowWidth;
    extern int windowHeight;
    extern Uint64 deltaTime;

    struct [[deprecated("Use `SDL_Color` instead.")]] RgbaColor {
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
        SDL_Color toRgbaColor(double hueSupplement=0.0) const;
        static double wrapHue(double hue, double const bound=360.0);
        void addHue(double const hueSupplement);
        std::string toString() const;
    };

    constexpr RgbaColor BLACK{0x00, 0x00, 0x00, 0xFF};
    constexpr RgbaColor PATH_COLOR = BLACK.withRed(0xFF);

    extern SquareGrid globalGrid;
    extern Vector2::HashMap<Sdl::RgbaColor> globalColorMap;

    void refreshPresentation();

    void drawSquareGrid(SquareGrid const &grid=Sdl::globalGrid, Vector2::HashMap<RgbaColor> const &colorMap=Sdl::globalColorMap);

    void drawPointyTopHexagon(
        float const size,
        SDL_FPoint const &center,
        SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
    );
    void drawPointyTopHexagon(
        SDL_FPoint const &center,
        float const width, float const height,
        SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
    );

    void mainLoop();
    void exitHandler();
}

namespace Project::Graphics {
    /////////
}

#endif
