#ifndef window_hpp
#define window_hpp

#include "simpleDirectmediaLayer.hpp"
#include "SquareMaze.hpp"
#include "color.hpp"

namespace Project::Media {
    extern SDL_Window *window;
    extern SDL_Renderer *renderer;
    extern int windowWidth;
    extern int windowHeight;
    extern std::function<void()> windowRefresher;

    void setRenderDrawColor(SDL_Color const &color);

    void drawSquareMaze(
        SquareMaze const &maze,
        SDL_FPoint const &position,
        float const width, float const height,
        Media::ColorTriplet const &mainColor,
        Media::ColorTriplet const &wallColor
    );

    /**
     * @note Draws hexagons with "pointy top" style. That is, the hexagons have a point northward.
     * 
     * @param center pixel location of center of maze
     * @param radius 
     * @param width pixel width
     * @param height pixel height
     * @param getColorTriplet
     */
    void drawHexagonMaze(
        SDL_FPoint const &center,
        int const radius,
        float const width, float const height,
        std::function<Media::ColorTriplet(int /* axis 1 */, int /* axis 2 */)> getColorTriplet
    );
}

#endif
