#ifndef Application_Window_hpp
#define Application_Window_hpp

#include "simple_directmedia_layer.hpp"
#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "color.hpp"

namespace App::Window {
	extern SDL_Window *window;
	extern SDL_Renderer *renderer;
	extern int windowWidth;
	extern int windowHeight;
	extern std::function<void()> windowRefresher;

	static constexpr float wallFramePercent = 0.35f;
	static_assert(wallFramePercent >= 0.0f and wallFramePercent <= 1.0f);

	inline void setRenderDrawColor(SDL_Color const &color) {
		SDL_SetRenderDrawColor(
			Window::renderer,
			color.r, color.g, color.b, color.a
		);
	}

	void drawQuadrilateral(
		SDL_FPoint const &northwestPoint, SDL_FPoint const &northeastPoint,
		SDL_FPoint const &southwestPoint, SDL_FPoint const &southeastPoint,
		SDL_Color  const &northwestColor, SDL_Color  const &northeastColor,
		SDL_Color  const &southwestColor, SDL_Color  const &southeastColor
	);

	void drawSquareMaze(
		MazeEngine::SquareMaze const &maze,
		SDL_FPoint const &position,
		float const width, float const height,
		ColorGetter const getMainColorTriplet,
		ColorTriplet const &wallColor
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
		MazeEngine::HexagonMaze const &maze,
		SDL_FPoint const &center,
		float const width, float const height,
		ColorGetter const getMainColorTriplet,
		ColorTriplet const &wallColor
	);

	void refresh();
}

#endif
