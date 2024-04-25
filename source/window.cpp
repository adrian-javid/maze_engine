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

constexpr static std::tuple<SDL_FPoint, SDL_FPoint, SDL_FPoint, SDL_FPoint> getRectanglePointList(
    SDL_FPoint const &position, float const width, float const height
) {return std::make_tuple(
    position,
    SDL_FPoint{position.x + width, position.y},
    SDL_FPoint{position.x, position.y + height},
    SDL_FPoint{position.x + width, position.y + height}
);}

static void drawQuadrilateral(
    SDL_FPoint const &northwestPoint, SDL_FPoint const &northeastPoint,
    SDL_FPoint const &southwestPoint, SDL_FPoint const &southeastPoint,
    SDL_Color const &northwestColor, SDL_Color const &northeastColor,
    SDL_Color const &southwestColor, SDL_Color const &southeastColor
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
    int const columnCount{maze.ColumnCount()};
    int const rowCount{maze.RowCount()};

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
            ] = getRectanglePointList(
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
                mainColor3, mainColor1
            );

            /* Draw walls. */

            static constexpr float wallFramePercent = 0.35f;
            static_assert(wallFramePercent >= 0.0f); static_assert(wallFramePercent <= 1.0f);

            auto const &&[
                innerNorthwestPoint, innerNortheastPoint,
                innerSouthwestPoint, innerSoutheastPoint
            ] = getRectanglePointList(
                {
                    Util::linearInterpolation(wallFramePercent, outerNorthwestPoint.x, outerNorthwestPoint.x + rectangleWidthHalf),
                    Util::linearInterpolation(wallFramePercent, outerNorthwestPoint.y, outerNorthwestPoint.y + rectangleHeightHalf)
                },
                rectangleWidth * (1.0f - wallFramePercent), rectangleHeight * (1.0f - wallFramePercent)
            );

            auto const &[wallColor1, wallColor2, wallColor3] = wallColorTriplet;

            if (maze.hasWall(key, SquareMaze::Direction::north)) drawQuadrilateral(
                outerNorthwestPoint, outerNortheastPoint,
                innerNorthwestPoint, innerNortheastPoint,
                wallColor1, wallColor2,
                wallColor1, wallColor2
            );

            if (maze.hasWall(key, SquareMaze::Direction::east)) drawQuadrilateral(
                innerNortheastPoint, outerNortheastPoint,
                innerSoutheastPoint, outerSoutheastPoint,
                wallColor2, wallColor2,
                wallColor3, wallColor3
            );

            if (maze.hasWall(key, SquareMaze::Direction::south)) drawQuadrilateral(
                innerSouthwestPoint, innerSoutheastPoint,
                outerSouthwestPoint, outerSoutheastPoint,
                wallColor2, wallColor3,
                wallColor2, wallColor3
            );

            if (maze.hasWall(key, SquareMaze::Direction::west)) drawQuadrilateral(
                outerNorthwestPoint, innerNorthwestPoint,
                outerSouthwestPoint, innerSouthwestPoint,
                wallColor1, wallColor1,
                wallColor2, wallColor2
            );

        }
    }
}

static void drawPointyTopHexagon(
    SDL_FPoint const &center,
    float const width, float const height,
    SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
) {

    float const halfWidth = width / 2.0f;
    float const halfHeight = height / 2.0f;
    float const quarterHeight = height / 4.0f;

    SDL_FPoint const topPoint{center.x, center.y - halfHeight};
    SDL_FPoint const topLeftPoint{center.x - halfWidth, center.y - quarterHeight};
    SDL_FPoint const topRightPoint{center.x + halfWidth, center.y - quarterHeight};

    SDL_FPoint const bottomPoint{center.x, center.y + halfHeight};
    SDL_FPoint const bottomLeftPoint{center.x - halfWidth, center.y + quarterHeight};
    SDL_FPoint const bottomRightPoint{center.x + halfWidth, center.y + quarterHeight};

    static constexpr SDL_FPoint zeroPoint = {0.0f, 0.0f};

    SDL_Vertex const topVertex{topPoint, firstColor, zeroPoint};
    SDL_Vertex const topLeftVertex{topLeftPoint, firstColor, zeroPoint};
    SDL_Vertex const topRightVertex{topRightPoint, secondColor, zeroPoint};

    SDL_Vertex const bottomLeftVertex{bottomLeftPoint, secondColor, zeroPoint};
    SDL_Vertex const bottomRightVertex{bottomRightPoint, thirdColor, zeroPoint};
    SDL_Vertex const bottomVertex{bottomPoint, thirdColor, zeroPoint};

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
    drawPointyTopHexagon(center, std::sqrt(3.0f) * size, 2.0f * size, firstColor, secondColor, thirdColor);
}

void Media::drawHexagonMaze(
    HexagonMaze const &maze,
    SDL_FPoint const &center,
    float const width, float const height,
    ColorGetter const getMainColorTriplet,
    Media::ColorTriplet const &wallColor
) {

    int const radius{maze.Radius()};

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

            // int const axis2{horizontalIndex - radius + verticalIndex};

            /* top hexagon */ {
                int const axis1{horizontalIndex - radius + verticalIndex};
                auto const &&[firstColor, secondColor, thirdColor] = getMainColorTriplet({axis1, topAxis2});
                drawPointyTopHexagon(
                    {hexagonCenterX, topHexagonCenterY},
                    hexagonWidth, hexagonHeight,
                    firstColor, secondColor, thirdColor
                );
            }

            /* bottom hexagon */ {
                int const axis1{horizontalIndex - radius};
                auto const &&[firstColor, secondColor, thirdColor] = getMainColorTriplet({axis1, bottomAxis2});
                drawPointyTopHexagon(
                    {hexagonCenterX, bottomHexagonCenterY},
                    hexagonWidth, hexagonHeight,
                    firstColor, secondColor, thirdColor
                );
            }
        }

    }

}
