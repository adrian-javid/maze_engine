#include "application/window.hpp"

namespace App::Window {
	static constexpr std::tuple<
		SDL_FPoint, SDL_FPoint,
		SDL_FPoint, SDL_FPoint
	> getQuadrilateralPointList(
		SDL_FPoint const &position, float const width, float const height
	) {return std::make_tuple(
		position, SDL_FPoint{position.x + width, position.y},
		SDL_FPoint{position.x, position.y + height}, SDL_FPoint{position.x + width, position.y + height}
	);}
}

void App::Window::drawSquareMaze(
	MazeEngine::SquareMaze const &maze,
	SDL_FPoint const &position,
	float const width, float const height,
	TileColorTripletGetter const tileColorTripletGetter,
	WallColorTripletGetter const wallColorTripletGetter
) {
	int const columnCount{maze.getColumnCount()};
	int const rowCount{maze.getRowCount()};

	float const rectangleWidth{width / static_cast<float>(columnCount)};
	float const rectangleHeight{height / static_cast<float>(rowCount)};

	float const rectangleWidthHalf{rectangleWidth / 2.0f};
	float const rectangleHeightHalf{rectangleHeight / 2.0f};

	for (MazeEngine::Vector2 key(0); key.value1 < rowCount; ++key.value1) {
		for (key.value2 = 0; key.value2 < columnCount; ++key.value2) {
			auto const &&[mainColor1, mainColor2, mainColor3]{tileColorTripletGetter(key)};

			auto const &&[
				outerNorthwestPoint, outerNortheastPoint,
				outerSouthwestPoint, outerSoutheastPoint
			]{getQuadrilateralPointList(
				/* northwest corner of rectangle */ {
					static_cast<float>(key.value2) * rectangleWidth + position.x,
					static_cast<float>(key.value1) * rectangleHeight + position.y
				},
				rectangleWidth, rectangleHeight
			)};

			// Draw main tile base.
			drawQuadrilateral(
				outerNorthwestPoint, outerNortheastPoint,
				outerSouthwestPoint, outerSoutheastPoint,
				mainColor1, mainColor2,
				mainColor2, mainColor3
			);

			/* Draw walls. */

			auto const &&[
				innerNorthwestPoint, innerNortheastPoint,
				innerSouthwestPoint, innerSoutheastPoint
			]{getQuadrilateralPointList(
				{
					linearInterpolation(wallFramePercent, outerNorthwestPoint.x, outerNorthwestPoint.x + rectangleWidthHalf),
					linearInterpolation(wallFramePercent, outerNorthwestPoint.y, outerNorthwestPoint.y + rectangleHeightHalf)
				},
				rectangleWidth * (1.0f - wallFramePercent), rectangleHeight * (1.0f - wallFramePercent)
			)};

			auto const &[wallColor1, wallColor2, wallColor3]{wallColorTripletGetter({})};

			if (maze.checkAdjacent(key, MazeEngine::SquareMaze::Direction::north).hasWall) drawQuadrilateral(
				outerNorthwestPoint, outerNortheastPoint,
				innerNorthwestPoint, innerNortheastPoint,
				wallColor1, wallColor2,
				wallColor1, wallColor2
			);

			if (maze.checkAdjacent(key, MazeEngine::SquareMaze::Direction::east).hasWall) drawQuadrilateral(
				innerNortheastPoint, outerNortheastPoint,
				innerSoutheastPoint, outerSoutheastPoint,
				wallColor2, wallColor2,
				wallColor3, wallColor3
			);

			if (maze.checkAdjacent(key, MazeEngine::SquareMaze::Direction::south).hasWall) drawQuadrilateral(
				innerSouthwestPoint, innerSoutheastPoint,
				outerSouthwestPoint, outerSoutheastPoint,
				wallColor2, wallColor3,
				wallColor2, wallColor3
			);

			if (maze.checkAdjacent(key, MazeEngine::SquareMaze::Direction::west).hasWall) drawQuadrilateral(
				outerNorthwestPoint, innerNorthwestPoint,
				outerSouthwestPoint, innerSouthwestPoint,
				wallColor1, wallColor1,
				wallColor2, wallColor2
			);

		}
	}
}
