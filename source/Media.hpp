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
    extern std::function<void()> windowRefresher;

    using ColorTriplet = std::tuple<SDL_Color, SDL_Color, SDL_Color>;

    std::string toString(SDL_Color const &color);

    void setRenderDrawColor(SDL_Color const &color);

    void drawGrid(
        SquareGrid const &squareGrid,
        SDL_FPoint const &position,
        float const width, float const height
    );

    struct HslaColor {
        double hue, saturation, luminance, alpha;
        constexpr HslaColor(
            double const hueValue,
            double const saturationValue=1.0,
            double const luminanceValue=0.5,
            double const alphaValue=1.0
        ): hue{hueValue}, saturation{saturationValue}, luminance{luminanceValue}, alpha{alphaValue} {}
        SDL_Color toRgbaColor(double hueSupplement=0.0) const;
        static double wrapHue(double hue, double const bound=360.0);
        void addHue(double const hueSupplement);
        std::string toString() const;
    };

    constexpr SDL_Color BLACK{0x00, 0x00, 0x00, 0xFF};
    constexpr SDL_Color pathColor = {0xFF, BLACK.g, BLACK.b, BLACK.a};

    // extern SquareGrid globalGrid;
    extern Vector2::HashMap<SDL_Color> globalColorMap;

    void refreshWindow_v0();

    void drawSquareGrid(SquareGrid const &grid, Vector2::HashMap<SDL_Color> const &colorMap=Media::globalColorMap);

    void drawRectangleGrid(
        SDL_FPoint const &position,
        int const rowCount, int columnCount,
        float const width, float const height,
        std::function<Media::ColorTriplet(int, int)> getColorTriplet
    );

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
