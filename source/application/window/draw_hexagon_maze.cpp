#include "application/window.hpp"

namespace App::Window {
	static constexpr std::tuple<
		SDL_FPoint, SDL_FPoint, SDL_FPoint,
		SDL_FPoint, SDL_FPoint, SDL_FPoint
	> getPointyTopHexagonPointList(
		SDL_FPoint const &center, float const width, float const height
	) {
		float const halfWidth{width / 2.0f};
		float const halfHeight{height / 2.0f};
		float const quarterHeight{height / 4.0f};

		SDL_FPoint const northPoint{center.x, center.y - halfHeight};
		SDL_FPoint const northwestPoint{center.x - halfWidth, center.y - quarterHeight};
		SDL_FPoint const northeastPoint{center.x + halfWidth, center.y - quarterHeight};

		SDL_FPoint const southPoint{center.x, center.y + halfHeight};
		SDL_FPoint const southwestPoint{center.x - halfWidth, center.y + quarterHeight};
		SDL_FPoint const southeastPoint{center.x + halfWidth, center.y + quarterHeight};

		return std::make_tuple(
			northwestPoint, northPoint, northeastPoint,
			southwestPoint, southPoint, southeastPoint
		);
	}

	static void drawHexagon(
		SDL_FPoint const &northwestPoint, SDL_FPoint const &northPoint, SDL_FPoint const &northeastPoint,
		SDL_FPoint const &southwestPoint, SDL_FPoint const &southPoint, SDL_FPoint const &southeastPoint,
		SDL_Color  const &northwestColor, SDL_Color  const &northColor, SDL_Color  const &northeastColor,
		SDL_Color  const &southwestColor, SDL_Color  const &southColor, SDL_Color  const &southeastColor
	) {
		static constexpr SDL_FPoint zeroPoint{0.0f, 0.0f};

		SDL_Vertex const topVertex{northPoint, northColor, zeroPoint};
		SDL_Vertex const topLeftVertex{northwestPoint, northwestColor, zeroPoint};
		SDL_Vertex const topRightVertex{northeastPoint, northeastColor, zeroPoint};

		SDL_Vertex const bottomLeftVertex{southwestPoint, southwestColor, zeroPoint};
		SDL_Vertex const bottomRightVertex{southeastPoint, southeastColor, zeroPoint};
		SDL_Vertex const bottomVertex{southPoint, southColor, zeroPoint};

		static constexpr int vertexCount{12};
		std::array<SDL_Vertex, vertexCount> const vertexList{
			// Top triangle.
			topVertex, topLeftVertex, topRightVertex,

			// Middle triangles, (rectangle).
			topLeftVertex, topRightVertex, bottomLeftVertex,
			topRightVertex, bottomLeftVertex, bottomRightVertex,

			// Bottom triangle.
			bottomLeftVertex, bottomRightVertex, bottomVertex,
		};

		SDL_RenderGeometry(Window::renderer, nullptr, vertexList.data(), vertexCount, nullptr, 0);
	}

	[[maybe_unused]]
	static void drawPointyTopHexagon(
		float const size,
		SDL_FPoint const &center,
		SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
	) {
		auto const &&[
			northwestPoint, northPoint, northeastPoint,
			southwestPoint, southPoint, southeastPoint
		]{getPointyTopHexagonPointList(center, std::sqrt(3.0f) * size, 2.0f * size)};
		drawHexagon(
			northwestPoint, northPoint, northeastPoint,
			southwestPoint, southPoint, southeastPoint,
			firstColor, secondColor, thirdColor,
			firstColor, secondColor, thirdColor
		);
	}
}

void App::Window::drawHexagonMaze(
	MazeEngine::HexagonMaze const &maze,
	SDL_FPoint const &center,
	float const width, float const height,
	TileColorTripletGetter const tileColorTripletGetter,
	WallColorTripletGetter const wallColorTripletGetter
) {
	int const radius{maze.getRadius()};

	// Radius of 0 draws 1 hexagon.
	assert(radius >= 0);

	int const diameter{radius + 1 + radius};
	float const diameterValue{static_cast<float>(diameter)};

	float const hexagonWidth{width / diameterValue};

	// For height, first hexagon counts as 1, other hexagons count as 3/4.
	float const hexagonHeight{height / (1.0f + (3.0f * (diameterValue - 1.0f)) / 4.0f)};

	float const threeQuartersHexagonHeight{(3 * hexagonHeight) / 4};
	float const halfHexagonWidth{hexagonWidth / 2};

	for (int verticalIndex{0}; verticalIndex <= radius; ++verticalIndex) {
		float const verticalIndexValue{static_cast<float>(verticalIndex)};

		float const topHexagonCenterY{center.y - verticalIndexValue * threeQuartersHexagonHeight};
		float const bottomHexagonCenterY{center.y + verticalIndexValue * threeQuartersHexagonHeight};

		float const horizontalOffset{verticalIndexValue * halfHexagonWidth};

		int const topAxis2{-verticalIndex};
		int const bottomAxis2{verticalIndex};

		for (int horizontalIndex{0}; horizontalIndex < diameter - verticalIndex; ++horizontalIndex) {
			float const hexagonCenterX{center.x + static_cast<float>(horizontalIndex - radius) * hexagonWidth + horizontalOffset};

			auto const drawTile([
				&tileColorTripletGetter, &wallColorTripletGetter,
				hexagonCenterX, hexagonWidth, hexagonHeight,
				&maze
			](MazeEngine::Vector2 const &tileKey, float const hexagonCenterY) -> void {
				auto const [tileColor1, tileColor2, tileColor3]{tileColorTripletGetter(tileKey)};
				auto const &&[
					outerNorthwestPoint, outerNorthPoint, outerNortheastPoint,
					outerSouthwestPoint, outerSouthPoint, outerSoutheastPoint
				]{getPointyTopHexagonPointList({hexagonCenterX, hexagonCenterY}, hexagonWidth, hexagonHeight)};

				drawHexagon(
					outerNorthwestPoint, outerNorthPoint, outerNortheastPoint,
					outerSouthwestPoint, outerSouthPoint, outerSoutheastPoint,
					tileColor1, tileColor1, tileColor2,
					tileColor2, tileColor3, tileColor3
				);

				auto const &&[
					innerNorthwestPoint, innerNorthPoint, innerNortheastPoint,
					innerSouthwestPoint, innerSouthPoint, innerSoutheastPoint
				]{getPointyTopHexagonPointList(
					{hexagonCenterX, hexagonCenterY},
					hexagonWidth * (1.0f - wallFramePercent),
					hexagonHeight * (1.0f - wallFramePercent)
				)};

				if (maze.checkAdjacent(tileKey, MazeEngine::HexagonMaze::Direction::northeast).hasWall) {
					auto const [wallColor1, wallColor2, wallColor3]{wallColorTripletGetter({})};
					drawQuadrilateral(
						outerNorthPoint, outerNortheastPoint,
						innerNorthPoint, innerNortheastPoint,
						wallColor1, wallColor2,
						wallColor1, wallColor2
					);
				}

				if (maze.checkAdjacent(tileKey, MazeEngine::HexagonMaze::Direction::east).hasWall) {
					auto const [wallColor1, wallColor2, wallColor3]{wallColorTripletGetter({})};
					drawQuadrilateral(
						innerNortheastPoint, outerNortheastPoint,
						innerSoutheastPoint, outerSoutheastPoint,
						wallColor2, wallColor2,
						wallColor3, wallColor3
					);
				}

				if (maze.checkAdjacent(tileKey, MazeEngine::HexagonMaze::Direction::southeast).hasWall) {
					auto const [wallColor1, wallColor2, wallColor3]{wallColorTripletGetter({})};
					drawQuadrilateral(
						innerSouthPoint, innerSoutheastPoint,
						outerSouthPoint, outerSoutheastPoint,
						wallColor1, wallColor3,
						wallColor1, wallColor3
					);
				}

				if (maze.checkAdjacent(tileKey, MazeEngine::HexagonMaze::Direction::southwest).hasWall) {
					auto const [wallColor1, wallColor2, wallColor3]{wallColorTripletGetter({})};
					drawQuadrilateral(
						outerSouthwestPoint, innerSouthwestPoint,
						outerSouthPoint, innerSouthPoint,
						wallColor2, wallColor2,
						wallColor1, wallColor1
					);
				}

				if (maze.checkAdjacent(tileKey, MazeEngine::HexagonMaze::Direction::west).hasWall) {
					auto const [wallColor1, wallColor2, wallColor3]{wallColorTripletGetter({})};
					drawQuadrilateral(
						outerNorthwestPoint, innerNorthwestPoint,
						outerSouthwestPoint, innerSouthwestPoint,
						wallColor3, wallColor3,
						wallColor2, wallColor2
					);
				}

				if (maze.checkAdjacent(tileKey, MazeEngine::HexagonMaze::Direction::northwest).hasWall) {
					auto const [wallColor1, wallColor2, wallColor3]{wallColorTripletGetter({})};
					drawQuadrilateral(
						outerNorthwestPoint, outerNorthPoint,
						innerNorthwestPoint, innerNorthPoint,
						wallColor3, wallColor1,
						wallColor3, wallColor1
					);
				}
			});

			/* top hexagon */
			drawTile({horizontalIndex - radius + verticalIndex, topAxis2}, topHexagonCenterY);

			/* bottom hexagon */
			drawTile({horizontalIndex - radius, bottomAxis2}, bottomHexagonCenterY);
		}

	}

}
