#include "application/window.hpp"

void App::Window::drawQuadrilateral(
	SDL_FPoint const &northwestPoint, SDL_FPoint const &northeastPoint,
	SDL_FPoint const &southwestPoint, SDL_FPoint const &southeastPoint,
	SDL_Color  const &northwestColor, SDL_Color  const &northeastColor,
	SDL_Color  const &southwestColor, SDL_Color  const &southeastColor
) {
	static constexpr SDL_FPoint zeroPoint{0, 0};

	SDL_Vertex const topLeftVertex{northwestPoint, northwestColor, zeroPoint};
	SDL_Vertex const topRightVertex{northeastPoint, northeastColor, zeroPoint};
	SDL_Vertex const bottomLeftVertex{southwestPoint, southwestColor, zeroPoint};
	SDL_Vertex const bottomRightVertex{southeastPoint, southeastColor, zeroPoint};

	static constexpr int vertexCount{2 * (3)};
	std::array<SDL_Vertex, vertexCount> const vertexList{
		// Top left triangle.
		topLeftVertex, topRightVertex, bottomLeftVertex,

		// Bottom right triangle.
		topRightVertex, bottomLeftVertex, bottomRightVertex,
	};

	SDL_RenderGeometry(Window::renderer, nullptr, vertexList.data(), vertexCount, nullptr, 0);
}
