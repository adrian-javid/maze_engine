namespace Project::Global {/*


    This program solves mazes!


*/}

#include "breadthFirstSearch.hpp"
#include "depthFirstSearch.hpp"
#include "greedyBestFirstSearch.hpp"
#include "aStarSearch.hpp"

#include "simpleDirectmediaLayer.hpp"
#include "window.hpp"
#include "SquareMaze.hpp"
#include "HexagonMaze.hpp"
#include "Util.hpp"
#include "AppParam.hpp"

#include <thread>
#include <mutex>
#include <chrono>

#include <algorithm>
#include <iostream>
#include <cassert>
#include <random>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace Project;

namespace Project::Global {
    using namespace std::chrono_literals;

    static Maze *maze = nullptr;
    static SquareMaze squareMaze;
    static HexagonMaze hexagonMaze;
    static Vector2 mazeStart(0, 0);
    static Vector2 mazeEnd(0, 0);
    static double percentageWrap(double const value) { return Util::wrapValue(value, 1.00); }
    static void refreshWindow();

    static std::chrono::milliseconds sleepTime = 0ms;
    static void delay() {
        std::this_thread::sleep_for(sleepTime);
    }

    static std::mutex tileInfoMutex;
    static Vector2::HashSet pathTileSet;
    static Vector2::HashSet markedTileSet;
}

static void Project::Global::refreshWindow() {
    constexpr Media::HslaColor startEndColor(91.0, 1.0, 0.49);
    constexpr Media::HslaColor pathTileColor(0.0);
    constexpr Media::HslaColor wallColor(240.0);
    constexpr Media::HslaColor markedTileColor(300.0);
    constexpr Media::HslaColor unmarkedTileColor(155.0);

    constexpr double zeroPercent{0.0};
    static double percentage{zeroPercent};
    double const deltaPercentage = static_cast<double>(Media::deltaTime) * 0.00011;

    percentage = Global::percentageWrap(percentage + deltaPercentage);
    assert(percentage >= 0.0); assert(percentage < 1.0);

    constexpr double hueDepth{45.0 + 5.0 + 5.0};
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
    Media::ColorTriplet const wallColorTriplet = getColorTriplet(wallColor);
    Media::ColorTriplet const markedTileColorTriplet = getColorTriplet(markedTileColor);
    Media::ColorTriplet const unmarkedTileColorTriplet = getColorTriplet(unmarkedTileColor);
    Media::ColorTriplet const startEndColorTriplet = getColorTriplet(startEndColor);

    float const windowWidthValue = static_cast<float>(Media::windowWidth);
    float const windowHeightValue = static_cast<float>(Media::windowHeight);

    auto const mainColorGetter = [
        &markedTileColorTriplet, &unmarkedTileColorTriplet, &pathTileColorTriplet,
        &startEndColorTriplet
    ](Vector2 const &key) -> Media::ColorTriplet {
        if (key == Global::mazeStart or key == Global::mazeEnd)
            return startEndColorTriplet;

        // TODO: Maybe use "try get lock" here to go faster?
        std::lock_guard const lock(tileInfoMutex);
        if (Global::pathTileSet.find(key) != Global::pathTileSet.end())
            return pathTileColorTriplet;
        else if (Global::markedTileSet.find(key) != Global::markedTileSet.end())
            return markedTileColorTriplet;
        else
            return unmarkedTileColorTriplet;
    };

    if (Global::maze == &Global::squareMaze) Media::drawSquareMaze(
        squareMaze,
        {0.0f, 0.0f},
        windowWidthValue, windowHeightValue,
        mainColorGetter, wallColorTriplet
    ); else if (Global::maze == &Global::hexagonMaze) Media::drawHexagonMaze(
        hexagonMaze,
        {windowWidthValue / 2.0f, windowHeightValue / 2.0f},
        windowWidthValue, windowHeightValue,
        mainColorGetter, wallColorTriplet
    ); else
        throw Global::maze;

    SDL_RenderPresent(Media::renderer);
}

int main(int const argc, char *argv[]) {
    auto const &config = AppParam::parseArgv(argc, argv);

    std::string const &gridType = config.at("grid").argument;
    int const mazeSize{AppParam::castArg<int>(config.at("size").argument)};
    unsigned int const seed{AppParam::castArg<unsigned int>(config.at("seed").argument)};
    std::string const &searchAlgorithmName = config.at("search").argument;
    Global::sleepTime = std::chrono::milliseconds(AppParam::castArg<unsigned int>(config.at("delay").argument));
    bool const mazeWrap = AppParam::castArg<bool>(config.at("wrap").argument);

    int constexpr mazeFillValue{0xFFu};

    // Create maze object with grid type.
    if (gridType == "square") {
        Global::maze = &(Global::squareMaze = SquareMaze(mazeSize, mazeSize, mazeFillValue));
        if (mazeWrap)
            Global::mazeEnd = {Global::squareMaze.getRowCount() / 2, Global::squareMaze.getColumnCount() / 2};
        else
            Global::mazeEnd = {Global::squareMaze.getRowCount() - 1, Global::squareMaze.getColumnCount() - 1};
    } else if (gridType == "hexagon") {
        Global::maze = &(Global::hexagonMaze = HexagonMaze(mazeSize, mazeFillValue));
        Global::mazeStart.value2 = -Global::hexagonMaze.getRadius();
        if (not mazeWrap) Global::mazeEnd.value2 = Global::hexagonMaze.getRadius();
    } else {
        Util::errOutLn("Unable to resolve grid type from string: `" + gridType + "`.");
    }

    // Generate the maze corridors.
    Global::maze->generate(seed, mazeWrap);

    static constexpr auto const processVertex = [](Vector2 const &vertex) -> bool {
        /* lock */ {
            std::lock_guard const lock(Global::tileInfoMutex);
            Global::markedTileSet.insert(vertex);
        }
        Global::delay();
        return vertex == Global::mazeEnd;
    };

    static std::function<Vector2::HashMap<Vector2>(void)> searchMaze = nullptr;

    // Get the search algorithm.
    if (searchAlgorithmName == "depth") {
        searchMaze = []() { return depthFirstSearch(*Global::maze, Global::mazeStart, processVertex); };
    } else if (searchAlgorithmName == "breadth" or searchAlgorithmName == "dijkstra") {
        searchMaze = []() { return breadthFirstSearch(*Global::maze, Global::mazeStart, processVertex); };
    } else if (searchAlgorithmName == "greedy") {
        searchMaze = []() { return greedyBestFirstSearch(*Global::maze, Global::mazeStart, Global::mazeEnd, processVertex); };
    } else if (searchAlgorithmName == "a_star") {
        searchMaze = []() { return aStarSearch(*Global::maze, Global::mazeStart, Global::mazeEnd, processVertex); };
    } else {
        Util::errOutLn("Unable to resolve graph search algorithm from string: `" + searchAlgorithmName + "`.");
    }
    assert(searchMaze != nullptr);

    static constexpr auto solveMaze = []() -> void {
        // Search for end of maze.
        auto const upTree = searchMaze();

        // Path tiles.
        for (
            auto edge(upTree.find(Global::mazeEnd));
            edge->first /* child vertex */ != Global::mazeStart;
            edge = upTree.find(edge->second /* parent vertex */)
        ) {
            /* lock */ {
                std::lock_guard const lock(Global::tileInfoMutex);
                Global::pathTileSet.insert(edge->first);
            }
            Global::delay();
        }

        /* lock */ {
            std::lock_guard const lock(Global::tileInfoMutex);
            Global::pathTileSet.insert(Global::mazeStart); // include corner
        }
    };

    #if true
    std::cout << "\n";
    for (auto const &[name, param] : config) {
        std::cout << name << " : " << param.argument << '\n';
    }
    #endif

    // Initialize the Simple Directmedia Layer library.
    SDL_Init(SDL_INIT_VIDEO);

    // Register exit handler.
    /*
        Note to self:

        I don't understand why
        calling this before `SDL_Init` causes a segmentation fault.

        I believe it has something to do with `SDL_Quit`.
    */
    std::atexit(&Media::exitHandler);

    SDL_CreateWindowAndRenderer(
        Media::windowWidth, Media::windowHeight,
        SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE,
        &Media::window, &Media::renderer
    );

    // Assert window and renderer were successfully created.
    assert(Media::window != nullptr);
    assert(Media::renderer != nullptr);

    SDL_SetWindowTitle(Media::window, "Maze Solver");
    SDL_SetWindowMinimumSize(Media::window, 250, 150);

    // Set the window refresher. This is called every iteration in the main loop.
    Media::windowRefresher = &Global::refreshWindow;

    // Start worker thread.
    std::thread const mazeSolver(solveMaze);

    // Start the main loop.
    #ifdef __EMSCRIPTEN__
    /*
        Note to self:

        `simulate_infinite_loop` is `true`, so will not continue execution after this function ends.

        "...if simulate_infinite_loop is false, and you created an object on the stack,
        it will be cleaned up before the main loop is called for the first time.""
        (https://emscripten.org/docs/api_reference/emscripten.h.html#id3)
    */
    emscripten_set_main_loop(&Media::mainLoop, -1, true);
    #else
    while (true) Media::mainLoop();
    #endif

    return EXIT_SUCCESS;
}
