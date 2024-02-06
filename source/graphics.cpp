#include "graphics.hpp"
#include <cmath>
#include <cassert>

#if true
#include <iostream>
static auto &O = std::cout;
#endif

using namespace Project;

SDL_Window *Sdl::window = nullptr;
SDL_Renderer *Sdl::renderer = nullptr;
int Sdl::windowWidth = 420;
int Sdl::windowHeight = 420;
Uint64 Sdl::deltaTime = 0;

void Sdl::RgbaColor::SetRenderDrawColor() const { SDL_SetRenderDrawColor(renderer, red, green, blue, alpha); }

std::string Sdl::RgbaColor::toString() const {
    std::stringstream buffer;
    buffer << "(R=" << +red << ", G=" << +green << ", B=" << +blue << ", A=" << +alpha << ")";
    return buffer.str();
}

SDL_Color Sdl::HslaColor::toRgbaColor(double hueSupplement) const {
    double const hueMix = HslaColor::wrapHue(hue + hueSupplement);

    assert(0 <= hueMix and hueMix < 360);
    assert(0 <= saturation and saturation <= 1);
    assert(0 <= luminance and saturation <= 1);
    assert(0 <= alpha and alpha <= 1);

    auto const chroma = (1.0 - std::fabs(2.0 * luminance - 1.0)) * saturation;

    auto const X = chroma * (1 - std::fabs(std::fmod(hueMix / 60.0, 2) - 1));

    struct Rgb { double r, g, b; } color{};

    /**/ if (  0 <= hueMix and hueMix <  60) color = Rgb{chroma,      X,    0.0}; 
    else if ( 60 <= hueMix and hueMix < 120) color = Rgb{     X, chroma,    0.0};
    else if (120 <= hueMix and hueMix < 180) color = Rgb{   0.0, chroma,      X};
    else if (180 <= hueMix and hueMix < 240) color = Rgb{   0.0,      X, chroma};
    else if (240 <= hueMix and hueMix < 300) color = Rgb{     X,    0.0, chroma};
    else if (300 <= hueMix and hueMix < 360) color = Rgb{chroma,    0.0,      X};

    auto const m = luminance - chroma / 2.0;

    return {
        static_cast<Uint8>((color.r + m) * 0xFF),
        static_cast<Uint8>((color.g + m) * 0xFF),
        static_cast<Uint8>((color.b + m) * 0xFF),
        static_cast<Uint8>((alpha      ) * 0xFF),
    };
}

double Sdl::HslaColor::wrapHue(double hue) {
    hue = std::fmod(hue, 360.0);
    if (hue < 0) hue += 360.0;
    if (hue >= 360.0) hue = 0.0;
    return hue;
}

void Sdl::HslaColor::addHue(double const hueSupplement) {
    hue = wrapHue(hue + hueSupplement);
}

std::string Sdl::HslaColor::toString() const {
    std::stringstream buffer;
    buffer << "(H=" << hue << ", S=" << saturation << ", L=" << luminance << ", A=" << alpha << ")";
    return buffer.str();
}

SquareGrid Sdl::grid;
Vector2::HashMap<Sdl::RgbaColor> Sdl::colorMap;

void Sdl::renderSquareGrid(SquareGrid const &grid, Vector2::HashMap<RgbaColor> const &colorMap) {
    static constexpr Sdl::RgbaColor wallColor{0x20, 0x20, 0x95, SDL_ALPHA_OPAQUE};
    static constexpr Sdl::RgbaColor defaultColor = wallColor.withGreen(wallColor.green * 5);

    int const rectangleWidth = windowWidth / Cast::toInt(grid.getColumnCount());
    int const rectangleHeight = windowHeight / Cast::toInt(grid.getRowCount());

    SDL_Rect rectangle{};
    rectangle.w = rectangleWidth;
    rectangle.h = rectangleHeight;

    int rowCount = Cast::toInt(grid.getRowCount());
    int columnCount = Cast::toInt(grid.getColumnCount());

    for (Vector2 vector(0, 0); vector.row < rowCount; ++vector.row) {
        for (vector.col = 0; vector.col < columnCount; ++vector.col) {
            rectangle.x = vector.col * rectangleWidth;
            rectangle.y = vector.row * rectangleHeight;

            if (colorMap.count(vector))
                colorMap.at(vector).SetRenderDrawColor();
            else if (grid.isWall(vector.row, vector.col))
                wallColor.SetRenderDrawColor();
            else
                defaultColor.SetRenderDrawColor();

            SDL_RenderFillRect(Sdl::renderer, &rectangle);
        }
    }
}

void Sdl::drawPointyTopHexagon(float const size, SDL_FPoint const &center, Sdl::HslaColor const &baseColor) {
    Sdl::drawPointyTopHexagon(center, std::sqrt(3.0f) * size, 2 * size, baseColor);
}

void Sdl::drawPointyTopHexagon(SDL_FPoint const &center, float const width, float const height, Sdl::HslaColor const &baseColor) {
    SDL_Color const firstColor = baseColor.toRgbaColor();
    SDL_Color const secondColor = baseColor.toRgbaColor(-20.0);
    SDL_Color const thirdColor = baseColor.toRgbaColor(-40.0);

    float const halfWidth = width / 2;
    float const halfHeight = height / 2;
    float const quarterHeight = height / 4;

    SDL_FPoint const topPoint{center.x, center.y - halfHeight};
    SDL_FPoint const topLeftPoint{center.x - halfWidth, center.y - quarterHeight};
    SDL_FPoint const topRightPoint{center.x + halfWidth, center.y - quarterHeight};

    SDL_FPoint const bottomPoint{center.x, center.y + halfHeight};
    SDL_FPoint const bottomLeftPoint{center.x - halfWidth, center.y + quarterHeight};
    SDL_FPoint const bottomRightPoint{center.x + halfWidth, center.y + quarterHeight};

    static constexpr SDL_FPoint zeroPoint = {0, 0};

    SDL_Vertex const topVertex{topPoint, firstColor, zeroPoint};
    SDL_Vertex const topLeftVertex{topLeftPoint, firstColor, zeroPoint};
    SDL_Vertex const topRightVertex{topRightPoint, secondColor, zeroPoint};

    SDL_Vertex const bottomLeftVertex{bottomLeftPoint, secondColor, zeroPoint};
    SDL_Vertex const bottomRightVertex{bottomRightPoint, thirdColor, zeroPoint};
    SDL_Vertex const bottomVertex{bottomPoint, thirdColor, zeroPoint};

    std::vector<SDL_Vertex> const vertexList = {
        // Top triangle.
        topVertex, topLeftVertex, topRightVertex,

        // Middle triangles, (rectangle).
        topLeftVertex, topRightVertex, bottomLeftVertex,
        topRightVertex, bottomLeftVertex, bottomRightVertex,

        // Bottom triangle.
        bottomLeftVertex, bottomRightVertex, bottomVertex,
   };

    SDL_RenderGeometry(Sdl::renderer, nullptr, vertexList.data(), Cast::toInt(vertexList.size()), nullptr, 0);
}

void Sdl::refreshPresentation() {
    static Sdl::HslaColor baseColor{240.0, 1.0, 0.5, 1.0};
    double const deltaHue = static_cast<double>(deltaTime) / 32.0; // TODO: need a safe cast from `Uint8` to `double`
    baseColor.addHue(deltaHue);
    // O << "delta time: " << deltaTime << ", " << baseColor.toString() << '\n';

    Sdl::BLACK.SetRenderDrawColor();
    SDL_RenderClear(Sdl::renderer);

    if (false) Sdl::renderSquareGrid();

    if (true) /* render hexagon grid */ {

        float hexagonWidth = 2.0f * (static_cast<float>(Sdl::windowWidth) / 5.0f);
        float hexagonHeight = 4.0f * (static_cast<float>(Sdl::windowHeight) / 7.0f);

        SDL_FPoint row0StartCenter{hexagonWidth, hexagonHeight / 2.0f};
        SDL_FPoint row1StartCenter{hexagonWidth / 2, hexagonHeight + hexagonHeight / 4.0f};

        Sdl::drawPointyTopHexagon(row0StartCenter, hexagonWidth, hexagonHeight, baseColor);
        Sdl::drawPointyTopHexagon(row1StartCenter, hexagonWidth, hexagonHeight, baseColor);
        Sdl::drawPointyTopHexagon(SDL_FPoint{row0StartCenter.x + hexagonWidth, row0StartCenter.y}, hexagonWidth, hexagonHeight, baseColor);
        Sdl::drawPointyTopHexagon(SDL_FPoint{row1StartCenter.x + hexagonWidth, row1StartCenter.y}, hexagonWidth, hexagonHeight, baseColor);

    }
    
    SDL_RenderPresent(Sdl::renderer);
}

void Sdl::mainLoop() {
    static Uint64 lastTime = 0;
    Uint64 const currentTime = SDL_GetTicks64();
    deltaTime = currentTime - lastTime;

    static SDL_Event event;
    while (SDL_PollEvent(&event)) switch (event.type) {
        case SDL_KEYDOWN: switch (event.key.keysym.sym) {
            case SDLK_BACKQUOTE:
                SDL_SetWindowFullscreen(Sdl::window, SDL_WINDOW_FULLSCREEN);
                break;
            case SDLK_ESCAPE:
                SDL_SetWindowFullscreen(Sdl::window, 0);
                break;
        } break;
        case SDL_WINDOWEVENT: switch (event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                Sdl::windowWidth = event.window.data1;
                Sdl::windowHeight = event.window.data2;
                break;
        } break;
        case SDL_QUIT:
            std::exit(EXIT_SUCCESS);
            break;
    }

    Sdl::refreshPresentation();

    static Uint64 timer = 0, counter = 0;
    if ((timer += deltaTime) >= 1 * 1000.0 /* one second */) {
        std::cout << "Hello " << ++counter << "." << '\n';
        timer = 0; // reset timer
    }

    lastTime = currentTime;
    SDL_Delay(1);
}

void Sdl::exitHandler() {
    if (Sdl::window) SDL_DestroyWindow(Sdl::window);
    if (Sdl::renderer) SDL_DestroyRenderer(Sdl::renderer);
    SDL_Quit();
}
