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
#include <random>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace Project;

namespace Project::Global {
    static SquareMaze maze0(20, 20, 0xFFu);
    static HexagonMaze maze1(10, 0xFFu);

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

int main(int const argc, char *argv[]) {
    using namespace std::string_literals;

    std::unordered_map<std::string, std::string> config {
        {"search"s, "depth"s },
        {"seed"s,   "0"s     },
        {"grid"s,   "square"s},
        {"size"s,   "5"s     },
        {"wrap"s,   "false"s },
    };

    for (int argIndex{0}; argIndex < argc; ++argIndex) {
        std::string const arg(argv[argIndex]);
        std::size_t const delimPos(arg.find("="s));
        if (delimPos == std::string::npos) {
            std::cout << "skip: " << arg << '\n';
            continue;
        }
        config[arg.substr(0, delimPos)] = arg.substr(delimPos+1);
    }
    std::cout << '\n';

    for (auto const &[key, value] : config) {
        std::cout << key << " = " << value << '\n';
    }

    std::exit(EXIT_SUCCESS);

    /*
        Example:
            ./solve_maze seed=1124134 search=breadth grid=hexagon wrap=true sound=true seed=1231

        seed -> natural number

        search -> string:
            depth
            breadth
            dijkstra (alias for breadth)
            a_star
            greedy

        grid -> string
            hexagon
            grid
    */

    // Mazify mazes.
    Global::maze0.shuffle(4);
    Global::maze1.shuffle(4);

    // Initialize the Simple Directmedia Layer library.
    SDL_Init(SDL_INIT_VIDEO);

    // Register exit handler.
    /*
        I don't understand why
        calling this before `SDL_Init` causes a segmentation fault.

        I believe it has something to do with `SDL_Quit`.
    */
    std::atexit(&Media::exitHandler);


    /*
        @TODO How should I handle this?

        If the start of the search is out of bounds, the start tile will not show as path.

        If the end of the search is out of bounds, the algorithm can't find the path to it.
    */

    // Search for a path that solves the maze.
    auto const path0 = breadthFirstSearch(
        Global::maze0,
        {Global::maze0.getRowCount() / 2, Global::maze0.getColumnCount() / 2}
        ,
        {Global::maze0.getRowCount() / 4, Global::maze0.getColumnCount() / 4}
    );

    // Save the path tiles.
    if (path0)
        for (auto const &vector : path0.value()) Global::pathTileSet0.insert(vector);

    auto const path1 = depthFirstSearch(Global::maze1, {0, 0}, {
        0, -Global::maze1.getRadius() / 2
    });

    if (path1)
        for (auto const &vector : path1.value()) Global::pathTileSet1.insert(vector);

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
