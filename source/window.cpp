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

static void drawRectangle(
    SDL_FPoint const &position,
    float const width, float const height,
    SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
) {
    SDL_FPoint const topRightPoint{position.x + width, position.y};
    SDL_FPoint const bottomLeftPoint{position.x, position.y + height};
    SDL_FPoint const bottomRightPoint{position.x + width, position.y + height};

    static constexpr SDL_FPoint zeroPoint = {0, 0};

    SDL_Vertex const topLeftVertex{position, firstColor, zeroPoint};
    SDL_Vertex const topRightVertex{topRightPoint, secondColor, zeroPoint};
    SDL_Vertex const bottomLeftVertex{bottomLeftPoint, secondColor, zeroPoint};
    SDL_Vertex const bottomRightVertex{bottomRightPoint, thirdColor, zeroPoint};

    constexpr int vertexCount = 2 * (3);
    std::array<SDL_Vertex, vertexCount> const vertexList = {
        // Top left triangle.
        topLeftVertex, topRightVertex, bottomLeftVertex,

        // Bottom right triangle.
        topRightVertex, bottomLeftVertex, bottomRightVertex,
    };

    SDL_RenderGeometry(Media::renderer, nullptr, vertexList.data(), vertexCount, nullptr, 0);
}

void Media::drawRectangleGrid(
    SDL_FPoint const &position,
    int const rowCount, int columnCount,
    float const width, float const height,
    std::function<Media::ColorTriplet(int, int)> getColorTriplet
) {
    float const rectangleWidth = width / static_cast<float>(columnCount);
    float const rectangleHeight = height / static_cast<float>(rowCount);

    for (int row = 0; row < rowCount; ++row) {
        for (int column = 0; column < columnCount; ++column) {
            auto const [firstColor, secondColor, thirdColor] = getColorTriplet(row, column);
            drawRectangle(
                {
                    static_cast<float>(column) * rectangleWidth + position.x,
                    static_cast<float>(row) * rectangleHeight + position.y
                },
                rectangleWidth, rectangleHeight,
                firstColor, secondColor, thirdColor
            );
        }
    }
}

void Media::drawGrid(
    SquareGrid const &squareGrid,
    SDL_FPoint const &position,
    float const width, float const height
) {
    drawRectangleGrid(
        position,
        squareGrid.RowCount(), squareGrid.ColumnCount(),
        width, height,
        [](int, int) -> Media::ColorTriplet {
            return {SDL_Color{}, SDL_Color{}, SDL_Color{}};
        }
    );
}

void Media::drawPointyTopHexagon(
    float const size,
    SDL_FPoint const &center,
    SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
) {
    Media::drawPointyTopHexagon(center, std::sqrt(3.0f) * size, 2.0f * size, firstColor, secondColor, thirdColor);
}

void Media::drawPointyTopHexagon(
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

static void drawPointyTopHexagonGrid(
    SDL_FPoint const &center,
    int const radius,
    float const width, float const height,
    SDL_Color const &firstColor, SDL_Color const &secondColor, SDL_Color const &thirdColor
) {

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

        for (int horizontalIndex = 0; horizontalIndex < diameter - verticalIndex; ++horizontalIndex) {
            float const hexagonCenterX = center.x + static_cast<float>(horizontalIndex - radius) * hexagonWidth + horizontalOffset;
            Media::drawPointyTopHexagon({hexagonCenterX, topHexagonCenterY}, hexagonWidth, hexagonHeight, firstColor, secondColor, thirdColor);
            Media::drawPointyTopHexagon({hexagonCenterX, bottomHexagonCenterY}, hexagonWidth, hexagonHeight, firstColor, secondColor, thirdColor);
        }
    }

}

[[deprecated]] static double asHue(
    double const hueOffset,
    double const hueLowerBound,
    double const hueUpperBound,
    double const colorCycleLength
) {
    if (hueOffset < colorCycleLength)
        return Media::HslaColor::wrapHue(hueLowerBound + hueOffset);
    else /* mirror */
        return Media::HslaColor::wrapHue(hueUpperBound - (hueOffset - colorCycleLength));
}

void Media::refreshWindow_v0() {
    static Media::HslaColor firstColor{0.0, 1.0, 0.5, 1.0};
    static Media::HslaColor secondColor = firstColor;
    static Media::HslaColor thirdColor = secondColor;

    static double hueOffset = 0.0;

    double constexpr hueLowerBound = 210.0;
    double constexpr hueUpperBound = 240.0;
    static_assert(0 <= hueLowerBound);
    static_assert(hueLowerBound < hueUpperBound);
    static_assert(hueUpperBound < 360.0);

    double constexpr colorCycleLength = hueUpperBound - hueLowerBound;
    static_assert(colorCycleLength > 0);

    double const deltaHue = static_cast<double>(deltaTime) / 32.0; // TODO: need a safe cast from `Uint8` to `double`

    firstColor.hue = asHue(
        hueOffset = Media::HslaColor::wrapHue(hueOffset + deltaHue, 2 * colorCycleLength),
        hueLowerBound, hueUpperBound, colorCycleLength
    );

    secondColor.hue = asHue(
        Media::HslaColor::wrapHue(hueOffset - 20.0, 2.0 * colorCycleLength),
        hueLowerBound, hueUpperBound, colorCycleLength
    );

    thirdColor.hue = asHue(
        Media::HslaColor::wrapHue(hueOffset - 40.0, 2.0 * colorCycleLength),
        hueLowerBound, hueUpperBound, colorCycleLength
    );

    Media::setRenderDrawColor(Media::black);
    SDL_RenderClear(Media::renderer);

    if (true) /* draw rectangle grid */ {
        drawRectangleGrid(
            /* position */ {0.0f, 0.0f},
            /* row count */ 5,
            /* column count */ 5,
            static_cast<float>(Media::windowWidth) / 2.0f,
            static_cast<float>(Media::windowHeight) / 2.0f,
            [](int, int) -> Media::ColorTriplet {return{
                firstColor.toRgbaColor(), secondColor.toRgbaColor(), thirdColor.toRgbaColor()
            };}
        );
        drawRectangleGrid(
            /* position */ {static_cast<float>(Media::windowWidth) / 2.0f, static_cast<float>(Media::windowHeight) / 2.0f},
            /* row count */ 5,
            /* column count */ 5,
            static_cast<float>(Media::windowWidth) / 2.0f,
            static_cast<float>(Media::windowHeight) / 2.0f,
            [](int, int) -> Media::ColorTriplet {
                return {
                    firstColor.toRgbaColor(), secondColor.toRgbaColor(), thirdColor.toRgbaColor()
                };
            }
        );
        drawPointyTopHexagonGrid(
            /* center */ {
                static_cast<float>(Media::windowWidth) * 3.0f / 4.0f,
                static_cast<float>(Media::windowHeight) * 1.0f / 4.0f
            },
            /* radius */ 3,
            static_cast<float>(Media::windowWidth) / 2.0f,
            static_cast<float>(Media::windowHeight) / 2.0f,
            firstColor.toRgbaColor(), secondColor.toRgbaColor(), thirdColor.toRgbaColor()
        );
        drawPointyTopHexagonGrid(
            /* center */ {
                static_cast<float>(Media::windowWidth) * 1.0f / 4.0f,
                static_cast<float>(Media::windowHeight) * 3.0f / 4.0f
            },
            /* radius */ 3,
            static_cast<float>(Media::windowWidth) / 2.0f,
            static_cast<float>(Media::windowHeight) / 2.0f,
            firstColor.toRgbaColor(), secondColor.toRgbaColor(), thirdColor.toRgbaColor()
        );
    }

    if (false) /* draw hexagon grid */ {
        drawPointyTopHexagonGrid(
            /* center */ {
                static_cast<float>(Media::windowWidth) * 3.0f / 4.0f,
                static_cast<float>(Media::windowHeight) * 2.0f / 4.0f
            },
            /* radius */ 3,
            static_cast<float>(Media::windowWidth) / 2.0f,
            static_cast<float>(Media::windowHeight),
            firstColor.toRgbaColor(), secondColor.toRgbaColor(), thirdColor.toRgbaColor()
        );
    }

    SDL_RenderPresent(Media::renderer);
}
