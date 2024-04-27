#include "window.hpp"

#include <array>
#include <cassert>

using namespace Project;

SDL_Window *Media::window = nullptr;
SDL_Renderer *Media::renderer = nullptr;
int Media::windowWidth = 420;
int Media::windowHeight = 420;
std::function<void()> Media::windowRefresher = []() constexpr -> void {};

void Media::setRenderDrawColor(SDL_Color const &color) {
    SDL_SetRenderDrawColor(
        Media::renderer,
        color.r, color.g, color.b, color.a
    );
}

constexpr static std::tuple<
    SDL_FPoint, SDL_FPoint,
    SDL_FPoint, SDL_FPoint
> getQuadrilateralPointList(
    SDL_FPoint const &position, float const width, float const height
) {return std::make_tuple(
    position, SDL_FPoint{position.x + width, position.y},
    SDL_FPoint{position.x, position.y + height}, SDL_FPoint{position.x + width, position.y + height}
);}

static void drawQuadrilateral(
    SDL_FPoint const &northwestPoint, SDL_FPoint const &northeastPoint,
    SDL_FPoint const &southwestPoint, SDL_FPoint const &southeastPoint,
    SDL_Color  const &northwestColor, SDL_Color  const &northeastColor,
    SDL_Color  const &southwestColor, SDL_Color  const &southeastColor
) {
    static constexpr SDL_FPoint zeroPoint = {0, 0};

    SDL_Vertex const topLeftVertex{northwestPoint, northwestColor, zeroPoint};
    SDL_Vertex const topRightVertex{northeastPoint, northeastColor, zeroPoint};
    SDL_Vertex const bottomLeftVertex{southwestPoint, southwestColor, zeroPoint};
    SDL_Vertex const bottomRightVertex{southeastPoint, southeastColor, zeroPoint};

    constexpr int vertexCount = 2 * (3);
    std::array<SDL_Vertex, vertexCount> const vertexList = {
        // Top left triangle.
        topLeftVertex, topRightVertex, bottomLeftVertex,

        // Bottom right triangle.
        topRightVertex, bottomLeftVertex, bottomRightVertex,
    };

    SDL_RenderGeometry(Media::renderer, nullptr, vertexList.data(), vertexCount, nullptr, 0);
}

void Media::drawSquareMaze(
    SquareMaze const &maze,
    SDL_FPoint const &position,
    float const width, float const height,
    ColorGetter const getMainColorTriplet,
    Media::ColorTriplet const &wallColorTriplet
) {
    int const columnCount{maze.getColumnCount()};
    int const rowCount{maze.getRowCount()};

    float const rectangleWidth = width / static_cast<float>(columnCount);
    float const rectangleHeight = height / static_cast<float>(rowCount);

    float const rectangleWidthHalf = rectangleWidth / 2.0f;
    float const rectangleHeightHalf = rectangleHeight / 2.0f;

    for (Vector2 key(0); key.value1 < rowCount; ++key.value1) {
        for (key.value2 = 0; key.value2 < columnCount; ++key.value2) {
            auto const &&[mainColor1, mainColor2, mainColor3] = getMainColorTriplet(key);

            auto const &&[
                outerNorthwestPoint, outerNortheastPoint,
                outerSouthwestPoint, outerSoutheastPoint
            ] = getQuadrilateralPointList(
                /* northwest corner of rectangle */ {
                    static_cast<float>(key.value2) * rectangleWidth + position.x,
                    static_cast<float>(key.value1) * rectangleHeight + position.y
                },
                rectangleWidth, rectangleHeight
            );

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
            ] = getQuadrilateralPointList(
                {
                    Util::linearInterpolation(wallFramePercent, outerNorthwestPoint.x, outerNorthwestPoint.x + rectangleWidthHalf),
                    Util::linearInterpolation(wallFramePercent, outerNorthwestPoint.y, outerNorthwestPoint.y + rectangleHeightHalf)
                },
                rectangleWidth * (1.0f - wallFramePercent), rectangleHeight * (1.0f - wallFramePercent)
            );

            auto const &[wallColor1, wallColor2, wallColor3] = wallColorTriplet;

            if (std::get<bool>(maze.checkAdjacent(key, SquareMaze::Direction::north))) drawQuadrilateral(
                outerNorthwestPoint, outerNortheastPoint,
                innerNorthwestPoint, innerNortheastPoint,
                wallColor1, wallColor2,
                wallColor1, wallColor2
            );

            if (std::get<bool>(maze.checkAdjacent(key, SquareMaze::Direction::east))) drawQuadrilateral(
                innerNortheastPoint, outerNortheastPoint,
                innerSoutheastPoint, outerSoutheastPoint,
                wallColor2, wallColor2,
                wallColor3, wallColor3
            );

            if (std::get<bool>(maze.checkAdjacent(key, SquareMaze::Direction::south))) drawQuadrilateral(
                innerSouthwestPoint, innerSoutheastPoint,
                outerSouthwestPoint, outerSoutheastPoint,
                wallColor2, wallColor3,
                wallColor2, wallColor3
            );

            if (std::get<bool>(maze.checkAdjacent(key, SquareMaze::Direction::west))) drawQuadrilateral(
                outerNorthwestPoint, innerNorthwestPoint,
                outerSouthwestPoint, innerSouthwestPoint,
                wallColor1, wallColor1,
                wallColor2, wallColor2
            );

        }
    }
}

constexpr static std::tuple<
    SDL_FPoint, SDL_FPoint, SDL_FPoint,
    SDL_FPoint, SDL_FPoint, SDL_FPoint
> getPointyTopHexagonPointList(
    SDL_FPoint const &center, float const width, float const height
) {
    float const halfWidth = width / 2.0f;
    float const halfHeight = height / 2.0f;
    float const quarterHeight = height / 4.0f;

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
    static constexpr SDL_FPoint zeroPoint = {0.0f, 0.0f};

    SDL_Vertex const topVertex{northPoint, northColor, zeroPoint};
    SDL_Vertex const topLeftVertex{northwestPoint, northwestColor, zeroPoint};
    SDL_Vertex const topRightVertex{northeastPoint, northeastColor, zeroPoint};

    SDL_Vertex const bottomLeftVertex{southwestPoint, southwestColor, zeroPoint};
    SDL_Vertex const bottomRightVertex{southeastPoint, southeastColor, zeroPoint};
    SDL_Vertex const bottomVertex{southPoint, southColor, zeroPoint};

    static constexpr int vertexCount{12};
    std::array<SDL_Vertex, vertexCount> const vertexList = {
        // Top triangle.
        topVertex, topLeftVertex, topRightVertex,

        // Middle triangles, (rectangle).
        topLeftVertex, topRightVertex, bottomLeftVertex,
        topRightVertex, bottomLeftVertex, bottomRightVertex,

        // Bottom triangle.
        bottomLeftVertex, bottomRightVertex, bottomVertex,
   };

    SDL_RenderGeometry(Media::renderer, nullptr, vertexList.data(), vertexCount, nullptr, 0);
}

static void drawPointyTopHexagon(
    float const size,
    SDL_FPoint const &center,
    SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
) {
    auto const &&[
        northwestPoint, northPoint, northeastPoint,
        southwestPoint, southPoint, southeastPoint
    ] = getPointyTopHexagonPointList(center, std::sqrt(3.0f) * size, 2.0f * size);
    drawHexagon(
        northwestPoint, northPoint, northeastPoint,
        southwestPoint, southPoint, southeastPoint,
        firstColor, secondColor, thirdColor,
        firstColor, secondColor, thirdColor
    );
}

void Media::drawHexagonMaze(
    HexagonMaze const &maze,
    SDL_FPoint const &center,
    float const width, float const height,
    ColorGetter const getMainColorTriplet,
    Media::ColorTriplet const &wallColorTriplet
) {
    int const radius{maze.getRadius()};
    auto const &[wallColor1, wallColor2, wallColor3] = wallColorTriplet;

    // Radius of 0 draws 1 hexagon.
    assert(radius >= 0);

    int const diameter = radius + 1 + radius;
    float const diameterValue = static_cast<float>(diameter);

    float const hexagonWidth = width / diameterValue;

    // For height, first hexagon counts as 1, other hexagons count as 3/4.
    float const hexagonHeight = height / (1.0f + (3.0f * (diameterValue - 1.0f)) / 4.0f);

    float const threeQuartersHexagonHeight = (3 * hexagonHeight) / 4;
    float const halfHexagonWidth = hexagonWidth / 2;

    for (int verticalIndex = 0; verticalIndex <= radius; ++verticalIndex) {
        float const verticalIndexValue = static_cast<float>(verticalIndex);

        float const topHexagonCenterY = center.y - verticalIndexValue * threeQuartersHexagonHeight;
        float const bottomHexagonCenterY = center.y + verticalIndexValue * threeQuartersHexagonHeight;

        float const horizontalOffset = verticalIndexValue * halfHexagonWidth;

        int const topAxis2{-verticalIndex};
        int const bottomAxis2{verticalIndex};

        for (int horizontalIndex = 0; horizontalIndex < diameter - verticalIndex; ++horizontalIndex) {
            float const hexagonCenterX = center.x + static_cast<float>(horizontalIndex - radius) * hexagonWidth + horizontalOffset;

            auto const drawTile = [
                &getMainColorTriplet, &wallColor1, &wallColor2, &wallColor3,
                hexagonCenterX, hexagonWidth, hexagonHeight,
                &maze
            ](Vector2 const &tileKey, float const hexagonCenterY) -> void {
                auto const &&[mainColor1, mainColor2, mainColor3] = getMainColorTriplet(tileKey);
                auto const &&[
                    outerNorthwestPoint, outerNorthPoint, outerNortheastPoint,
                    outerSouthwestPoint, outerSouthPoint, outerSoutheastPoint
                ] = getPointyTopHexagonPointList({hexagonCenterX, hexagonCenterY}, hexagonWidth, hexagonHeight);
                drawHexagon(
                    outerNorthwestPoint, outerNorthPoint, outerNortheastPoint,
                    outerSouthwestPoint, outerSouthPoint, outerSoutheastPoint,
                    mainColor1, mainColor1, mainColor2,
                    mainColor2, mainColor3, mainColor3
                );

                auto const &&[
                    innerNorthwestPoint, innerNorthPoint, innerNortheastPoint,
                    innerSouthwestPoint, innerSouthPoint, innerSoutheastPoint
                ] = getPointyTopHexagonPointList(
                    {hexagonCenterX, hexagonCenterY},
                    hexagonWidth * (1.0f - wallFramePercent),
                    hexagonHeight * (1.0f - wallFramePercent)
                );

                if (std::get<bool>(maze.checkAdjacent(tileKey, HexagonMaze::Direction::northeast))) drawQuadrilateral(
                    outerNorthPoint, outerNortheastPoint,
                    innerNorthPoint, innerNortheastPoint,
                    wallColor1, wallColor2,
                    wallColor1, wallColor2
                );

                if (std::get<bool>(maze.checkAdjacent(tileKey, HexagonMaze::Direction::east))) drawQuadrilateral(
                    innerNortheastPoint, outerNortheastPoint,
                    innerSoutheastPoint, outerSoutheastPoint,
                    wallColor2, wallColor2,
                    wallColor3, wallColor3
                );

                if (std::get<bool>(maze.checkAdjacent(tileKey, HexagonMaze::Direction::southeast))) drawQuadrilateral(
                    innerSouthPoint, innerSoutheastPoint,
                    outerSouthPoint, outerSoutheastPoint,
                    wallColor1, wallColor3,
                    wallColor1, wallColor3
                );

                if (std::get<bool>(maze.checkAdjacent(tileKey, HexagonMaze::Direction::southwest))) drawQuadrilateral(
                    outerSouthwestPoint, innerSouthwestPoint,
                    outerSouthPoint, innerSouthPoint,
                    wallColor2, wallColor2,
                    wallColor1, wallColor1
                );

                if (std::get<bool>(maze.checkAdjacent(tileKey, HexagonMaze::Direction::west))) drawQuadrilateral(
                    outerNorthwestPoint, innerNorthwestPoint,
                    outerSouthwestPoint, innerSouthwestPoint,
                    wallColor3, wallColor3,
                    wallColor2, wallColor2
                );

                if (std::get<bool>(maze.checkAdjacent(tileKey, HexagonMaze::Direction::northwest))) drawQuadrilateral(
                    outerNorthwestPoint, outerNorthPoint,
                    innerNorthwestPoint, innerNorthPoint,
                    wallColor3, wallColor1,
                    wallColor3, wallColor1
                );
            };

            /* top hexagon */
            drawTile({horizontalIndex - radius + verticalIndex, topAxis2}, topHexagonCenterY);

            /* bottom hexagon */
            drawTile({horizontalIndex - radius, bottomAxis2}, bottomHexagonCenterY);
        }

    }

}
