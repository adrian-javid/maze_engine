#ifndef graphics_hpp
#define graphics_hpp

#ifdef _WIN64
#include "SDL2/SDL.h"
#else
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wold-style-cast"
#include <SDL.h>
#pragma GCC diagnostic pop
#endif

#include "SquareGrid.hpp"
#include "Vector2.hpp"

namespace Project::Media {

    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern int windowWidth;
    extern int windowHeight;
    extern Uint64 deltaTime;

    std::string toString(SDL_Color const &color);

    void setRenderDrawColor(SDL_Color const &color);

    struct HslaColor {
        double hue, saturation, luminance, alpha;
        SDL_Color toRgbaColor(double hueSupplement=0.0) const;
        static double wrapHue(double hue, double const bound=360.0);
        void addHue(double const hueSupplement);
        std::string toString() const;
    };

    constexpr SDL_Color BLACK{0x00, 0x00, 0x00, 0xFF};
    constexpr SDL_Color PATH_COLOR = {0xFF, BLACK.g, BLACK.b, BLACK.a};

    extern SquareGrid globalGrid;
    extern Vector2::HashMap<SDL_Color> globalColorMap;

    void refreshPresentation();

    void drawSquareGrid(SquareGrid const &grid=Media::globalGrid, Vector2::HashMap<SDL_Color> const &colorMap=Media::globalColorMap);

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

#endif
