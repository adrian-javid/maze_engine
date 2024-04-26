namespace Project::Global {/*


    This program solves mazes!


*/}

#include "breadthFirstSearch.hpp"
#include "depthFirstSearch.hpp"
#include "simpleDirectmediaLayer.hpp"
#include "window.hpp"
#include "SquareMaze.hpp"
#include "HexagonMaze.hpp"
#include "Util.hpp"

#include <algorithm>
#include <iostream>
#include <cassert>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>

#endif
#if true
static auto &o = std::cout;
static constexpr char ln = '\n';
#endif

using namespace Project;

namespace Project::Global {static SquareMaze generateGrid(int rowCount, int columnCount) {
    SquareMaze maze(rowCount, columnCount);
    int const secondQuarter = maze.getColumnCount() / 4;
    int const fourthQuarter = secondQuarter * 3;
    for (int row{0}; row < maze.getRowCount(); ++row) {
        // maze.putWall(row, secondQuarter);
        // maze.putWall(row, fourthQuarter);
        
        // maze.putWall(row, 0);
        // maze.putWall(row, maze.getColumnCount() - 1);
    }

    Vector2 key;

    maze.at(key={5, 5}) |= Maze::northWall | Maze::eastWall;

    maze.at(key={-3, -3}) |= Maze::northWall | Maze::eastWall;
    maze.at(key + Vector2::squareSouth) |= Maze::northWall;
    maze.at(key + Vector2::squareWest) |= Maze::eastWall;

    maze.at({(maze.getRowCount() - 1) - 1, secondQuarter}) = SquareMaze::emptyTile;
    maze.at({1, fourthQuarter}) = SquareMaze::emptyTile;

    for (int offset = 0; offset < 8; ++offset) {
        // maze.putWall(16, secondQuarter + offset);
        // maze.putWall(3, fourthQuarter - offset);
    }

    maze.at({(maze.getRowCount() - 1) - 2, secondQuarter}) = SquareMaze::emptyTile;
    maze.at({2, fourthQuarter}) = SquareMaze::emptyTile;

    for (int col{0}; col < maze.getRowCount(); ++col) {
        // maze.putWall(0, col);
        // maze.putWall(maze.getColumnCount() - 1, col);
    }

    return maze;
}}

namespace Project::Global {
    static auto const maze0 = generateGrid(20, 20);

    Vector2 const start0 = {0 + 1, 0 + 1};
    Vector2 const end0 = {(maze0.getRowCount() - 1) - 1, (maze0.getColumnCount() - 1) - 1};

    static auto const maze1 = []() -> HexagonMaze {
        HexagonMaze maze(4);
        auto const center = Vector2(0, 0);

        maze.at(center) |= HexagonMaze::northeastWall;
        maze.at(center + Vector2::hexagonSouthwest) |= HexagonMaze::northeastWall;

        maze.at(center) |= HexagonMaze::eastWall;
        maze.at(center + Vector2::hexagonWest) |= HexagonMaze::eastWall;

        maze.at(center) |= HexagonMaze::southeastWall;
        maze.at(center + Vector2::hexagonNorthwest) |= HexagonMaze::southeastWall;

        return maze;
    }();
    static Vector2::HashSet pathTileSet0;
    static Vector2::HashSet pathTileSet1;
    static double percentageWrap(double const value) { return Util::wrapValue(value, 1.00); }
}

namespace Project::Global {static void refreshWindow() {
    constexpr Media::HslaColor pathTileColor(0.0);
    constexpr Media::HslaColor wallTileColor(240.0);
    constexpr Media::HslaColor emptyTileColor(155.0);

    constexpr double zeroPercent{0.0};    
    static double percentage{zeroPercent};
    double const deltaPercentage = static_cast<double>(Media::deltaTime) * 0.00010;

    percentage = Global::percentageWrap(percentage + deltaPercentage);
    assert(percentage >= 0.0); assert(percentage < 1.0);

    constexpr double hueDepth{45.0};
    constexpr auto getColorTriplet = [](Media::HslaColor const &tileColor) -> Media::ColorTriplet {
        constexpr auto getCyclicHue = [](double const hue, double const percentageAddend) -> double {
            return Media::HslaColor::getCyclicHue(hue, Global::percentageWrap(percentage + percentageAddend), hueDepth);
        };
        return std::make_tuple(
            tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.00)),
            tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.10)),
            tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.20))
        );
    };

    Media::ColorTriplet const pathTileColorTriplet = getColorTriplet(pathTileColor);
    Media::ColorTriplet const wallTileColorTriplet = getColorTriplet(wallTileColor);
    Media::ColorTriplet const emptyTileColorTriplet = getColorTriplet(emptyTileColor);

    float const windowWidthValue = static_cast<float>(Media::windowWidth);
    float const windowHeightValue = static_cast<float>(Media::windowHeight);

    Media::drawSquareMaze(
        Global::maze0,
        {0.0f, 0.0f},
        windowWidthValue / 2.0f,
        windowHeightValue,
        [&emptyTileColorTriplet, &pathTileColorTriplet](Vector2 const &key) -> Media::ColorTriplet {
            if (Global::pathTileSet0.find(key) != Global::pathTileSet0.end())
                return pathTileColorTriplet;

            return emptyTileColorTriplet;
        },
        wallTileColorTriplet
    );

    Media::drawHexagonMaze(
        maze1,
        {3.0f * windowWidthValue / 4.0f, 1.0f * windowHeightValue / 2.0f},
        windowWidthValue / 2.0f, windowHeightValue,
        [
            &pathTileColorTriplet, &emptyTileColorTriplet
        ](Vector2 const &key) -> Media::ColorTriplet {
            if (Global::pathTileSet1.find(key) != Global::pathTileSet1.end())
                return pathTileColorTriplet;
            else
                return emptyTileColorTriplet;
        },
        wallTileColorTriplet
    );

    SDL_RenderPresent(Media::renderer);
}}

int main(int argc, char *argv[]) {
    static_cast<void>(argc); static_cast<void>(argv);

    #if false
    o << sizeof(std::variant<
        decltype(HexagonMaze().getTable().begin()),
        decltype(SquareMaze().getTable().begin())
    >) << ln;
    o << sizeof(std::any) << ln;
    o << typeid(void *).name() << ' ' << sizeof(void *) << ln;
    o << sizeof(std::function<Vector2 const &(std::any const &)>) << ln;
    o << sizeof(std::function<void()>) << ln;
    o << sizeof(std::ptrdiff_t) << ln;
    o << typeid(std::size_t).name() << ' ' << sizeof(std::size_t) << ln;
    #endif

    #if false
    int const radius = 4;
    float const width = 250.0f;
    float const height = 150.0f;
    SDL_FPoint const center{width / 2.0f, height / 2.0f};

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

        o << "axis 2: " << -verticalIndex << ", " << verticalIndex << '\n';

        for (int horizontalIndex = 0; horizontalIndex < diameter - verticalIndex; ++horizontalIndex) {
            float const hexagonCenterX = center.x + static_cast<float>(horizontalIndex - radius) * hexagonWidth + horizontalOffset;
            o << "\taxis 1: "
            << horizontalIndex - radius + verticalIndex
            << ", " << horizontalIndex - radius
            << '\n';
        }
    }

    o << "done\n";
    return EXIT_SUCCESS;
    #endif

    // Initialize the Simple Directmedia Layer library.
    SDL_Init(SDL_INIT_VIDEO);

    // Register exit handler.
    /*
        I don't understand why
        calling this before `SDL_Init` causes a segmentation fault.

        I believe it has something to do with `SDL_Quit`.
    */
    std::atexit(&Media::exitHandler);

    // Search for a path that solves the maze.
    auto const path0 = depthFirstSearch(Global::maze0, Global::start0, Global::end0);

    // Save the path tiles.
    if (path0)
        for (auto const &vector : path0.value()) Global::pathTileSet0.insert(vector);

    auto const path1 = breadthFirstSearch(Global::maze1, {-1, -1}, {0, 2});
    for (auto const &vector : path1.value()) Global::pathTileSet1.insert(vector);

    // You couldn't have guessed that this creates the window and renderer.
    SDL_CreateWindowAndRenderer(
        Media::windowWidth, Media::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &Media::window, &Media::renderer
    );

    // Assert window and renderer were created.
    assert(Media::window != nullptr);
    assert(Media::renderer != nullptr);

    SDL_SetWindowTitle(Media::window, "Maze Solver");
    SDL_SetWindowMinimumSize(Media::window, 250, 150);

    // Set the window refresher. This is called every iteration in the main loop.
    Media::windowRefresher = &Global::refreshWindow;

    // Start the main loop.
    #ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(&Media::mainLoop, -1, true);
    #else
    while (true) Media::mainLoop();
    #endif

    return EXIT_SUCCESS;
}
