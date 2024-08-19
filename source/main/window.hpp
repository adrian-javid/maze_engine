#ifndef main_WINDOW_HPP
#define main_WINDOW_HPP

#include "simple_directmedia_layer.hpp"
#include "core/maze/square.hpp"
#include "core/maze/hexagon.hpp"
#include "color.hpp"

namespace Project::Media {
	extern SDL_Window *window;
	extern SDL_Renderer *renderer;
	extern int windowWidth;
	extern int windowHeight;
	extern std::function<void()> windowRefresher;

	static constexpr float wallFramePercent = 0.35f;
	static_assert(wallFramePercent >= 0.0f); static_assert(wallFramePercent <= 1.0f);

	void setRenderDrawColor(SDL_Color const &color);

	void drawSquareMaze(
		SquareMaze const &maze,
		SDL_FPoint const &position,
		float const width, float const height,
		ColorGetter const getMainColorTriplet,
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
		HexagonMaze const &maze,
		SDL_FPoint const &center,
		float const width, float const height,
		ColorGetter const getMainColorTriplet,
		Media::ColorTriplet const &wallColor
	);
}

#endif
