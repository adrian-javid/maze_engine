#ifndef window_hpp
#define window_hpp

#include "simpleDirectmediaLayer.hpp"
#include "SquareGrid.hpp"
#include "color.hpp"

namespace Project::Media {
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern int windowWidth;
    extern int windowHeight;
    extern std::function<void()> windowRefresher;

    void setRenderDrawColor(SDL_Color const &color);

    void drawGrid(
        SquareGrid const &squareGrid,
        SDL_FPoint const &position,
        float const width, float const height
    );

    [[deprecated]] void refreshWindow_v0();

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
}


#endif
