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
#include "AppParam.hpp"

#include <algorithm>
#include <iostream>
#include <cassert>
#include <random>
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace Project;

namespace Project::Global {
    static Maze *maze = nullptr;
    static SquareMaze squareMaze;
    static HexagonMaze hexagonMaze;
    static Vector2::HashSet pathTileSet;
    static double percentageWrap(double const value) { return Util::wrapValue(value, 1.00); }
    static void refreshWindow();
}

static void Project::Global::refreshWindow() {
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

    auto const mainColorGetter = [&emptyTileColorTriplet, &pathTileColorTriplet](Vector2 const &key) -> Media::ColorTriplet {
        if (Global::pathTileSet.find(key) != Global::pathTileSet.end())
            return pathTileColorTriplet;
        else
            return emptyTileColorTriplet;
    };

    if (Global::maze == &Global::squareMaze) Media::drawSquareMaze(
        squareMaze,
        {0.0f, 0.0f},
        windowWidthValue, windowHeightValue,
        mainColorGetter, wallTileColorTriplet
    ); else if (Global::maze == &Global::hexagonMaze) Media::drawHexagonMaze(
        hexagonMaze,
        {windowWidthValue / 2.0f, windowHeightValue / 2.0f},
        windowWidthValue, windowHeightValue,
        mainColorGetter, wallTileColorTriplet
    ); else
        throw Global::maze;

    SDL_RenderPresent(Media::renderer);
}

int main(int const argc, char *argv[]) {
    auto const &config = AppParam::parseArgv(argc, argv);

    std::string const &gridType = config.at("grid").argument;

    int const mazeSize{AppParam::castArg<int>(config.at("size").argument)};
    int constexpr mazeFillValue{0xFFu};

    Vector2 mazeStart(0, 0);
    Vector2 mazeEnd(0, 0);

    if (gridType == "square") {
        Global::maze = &(Global::squareMaze = SquareMaze(mazeSize, mazeSize, mazeFillValue));
        mazeEnd = {Global::squareMaze.getRowCount() / 2, Global::squareMaze.getColumnCount() / 2};
    } else if (gridType == "hexagon") {
        Global::maze = &(Global::hexagonMaze = HexagonMaze(mazeSize, mazeFillValue));
        mazeStart.value2 = -Global::hexagonMaze.getRadius();
    } else {
        Util::errOutLn("Unable to resolve grid type from string: `" + gridType + "`.");
    }

    unsigned int const seed{AppParam::castArg<unsigned int>(config.at("seed").argument)};
    Global::maze->shuffle(seed);

    std::string const &searchAlgorithmName = config.at("search").argument;

    std::optional<std::vector<Vector2>> path;

    constexpr auto const forThisVertex = [](Vector2 const &vertex) -> bool {
        return {};
    };

    if (searchAlgorithmName == "depth") {
        depthFirstSearch(*Global::maze, mazeStart, forThisVertex);
    } else if (searchAlgorithmName == "breadth") {
        breadthFirstSearch(*Global::maze, mazeStart, forThisVertex);
    } else {
        Util::errOutLn("Unable to resolve graph search algorithm from string: `" + searchAlgorithmName + "`.");
    }

    if (path)
        for (Vector2 const &tileKey : path.value())
            Global::pathTileSet.insert(tileKey);

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
