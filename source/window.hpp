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

    void drawRectangleGrid(
        SDL_FPoint const &position,
        int const rowCount, int columnCount,
        float const width, float const height,
        std::function<Media::ColorTriplet(int /* row */, int /* column */)> getColorTriplet
    );

    void drawPointyTopHexagonGrid(
        SDL_FPoint const &center,
        int const radius,
        float const width, float const height,
        std::function<Media::ColorTriplet(int /* axis 1 */, int /* axis 2 */)> getColorTriplet
    );
}

#endif
