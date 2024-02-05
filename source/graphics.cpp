#include "graphics.hpp"
#include <cmath>

using namespace Project;

SDL_Window *Sdl::window = nullptr;
SDL_Renderer *Sdl::renderer = nullptr;
int Sdl::windowWidth = 420;
int Sdl::windowHeight = 420;
SDL_Event Sdl::event;

void Sdl::RgbaColor::SetRenderDrawColor() const { SDL_SetRenderDrawColor(renderer, red, green, blue, alpha); }

std::string Sdl::RgbaColor::toString() const {
    std::stringstream buffer;
    buffer << "(R=" << red << ", G=" << green << ", B=" << blue << ", A=" << alpha << ")";
    return buffer.str();
}

Sdl::RgbaColor Sdl::HslaColor::toRgbaColor() const {

    auto const chroma = (1 - std::fabs(2 * luminance - 1)) * saturation;

    auto const X = chroma * (1 - std::fabs(std::fmod(hue / 60.0, 2) - 1));

    struct Rgb { double r, g, b; } color{};

    /**/ if (  0 <= hue and hue <  60) color = Rgb{chroma,      X,    0.0}; 
    else if ( 60 <= hue and hue < 120) color = Rgb{     X, chroma,    0.0};
    else if (120 <= hue and hue < 180) color = Rgb{   0.0, chroma,      X};
    else if (180 <= hue and hue < 240) color = Rgb{   0.0,      X, chroma};
    else if (240 <= hue and hue < 300) color = Rgb{     X,    0.0, chroma};
    else if (300 <= hue and hue < 360) color = Rgb{chroma,    0.0,      X};

    auto const m = luminance - chroma / 2.0;

    return {
        static_cast<Uint8>((color.r + m) * 0xFF),
        static_cast<Uint8>((color.g + m) * 0xFF),
        static_cast<Uint8>((color.b + m) * 0xFF),
        static_cast<Uint8>((      alpha) * 0xFF)
    };
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

void Sdl::drawPointyTopHexagon(float const size, SDL_FPoint const &center) {
    Sdl::drawPointyTopHexagon(center, std::sqrt(3.0f) * size, 2 * size);
}

void Sdl::drawPointyTopHexagon(SDL_FPoint const &center, float const width, float const height) {
    float const halfWidth = width / 2;
    float const halfHeight = height / 2;
    float const quarterHeight = height / 4;

    SDL_FPoint const topPoint{center.x, center.y - halfHeight};
    SDL_FPoint const topLeftPoint{center.x - halfWidth, center.y - quarterHeight};
    SDL_FPoint const topRightPoint{center.x + halfWidth, center.y - quarterHeight};

    SDL_FPoint const bottomPoint{center.x, center.y + halfHeight};
    SDL_FPoint const bottomLeftPoint{center.x - halfWidth, center.y + quarterHeight};
    SDL_FPoint const bottomRightPoint{center.x + halfWidth, center.y + quarterHeight};

    static constexpr SDL_FPoint defaultNormalizedTextureCoordinates = {0, 0};

    std::vector<SDL_Vertex> const vertexList = {
        // Top triangle.
        {topPoint        , SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {topLeftPoint    , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {topRightPoint   , SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},

        // Middle triangles, (rectangle).
        {topLeftPoint    , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {topRightPoint   , SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {bottomLeftPoint , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {topRightPoint   , SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {bottomLeftPoint , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {bottomRightPoint, SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},

        // Bottom triangle.
        {bottomPoint     , SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {bottomLeftPoint , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
        {bottomRightPoint, SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, defaultNormalizedTextureCoordinates},
   };

    SDL_RenderGeometry(Sdl::renderer, nullptr, vertexList.data(), Cast::toInt(vertexList.size()), nullptr, 0);
}

void Sdl::refreshPresentation() {
    Sdl::BLACK.SetRenderDrawColor();
    SDL_RenderClear(Sdl::renderer);

    if (false) Sdl::renderSquareGrid();
    
    SDL_FPoint const center{
        static_cast<float>(Sdl::windowWidth) / 2.0f,
        static_cast<float>(Sdl::windowHeight) / 2.0f
    };
    static_cast<void>(center);

    if (true) /* render hexagon grid */ {

        float hexagonWidth = 2.0f * (static_cast<float>(Sdl::windowWidth) / 5.0f);
        float hexagonHeight = 4.0f * (static_cast<float>(Sdl::windowHeight) / 7.0f);

        SDL_FPoint row0StartCenter{hexagonWidth, hexagonHeight / 2.0f};
        SDL_FPoint row1StartCenter{hexagonWidth / 2, hexagonHeight + hexagonHeight / 4.0f};

        Sdl::drawPointyTopHexagon(row0StartCenter, hexagonWidth, hexagonHeight);
        Sdl::drawPointyTopHexagon(row1StartCenter, hexagonWidth, hexagonHeight);
        Sdl::drawPointyTopHexagon(SDL_FPoint{row0StartCenter.x + hexagonWidth, row0StartCenter.y}, hexagonWidth, hexagonHeight);
        Sdl::drawPointyTopHexagon(SDL_FPoint{row1StartCenter.x + hexagonWidth, row1StartCenter.y}, hexagonWidth, hexagonHeight);

    }
    
    SDL_RenderPresent(Sdl::renderer);
}

void Sdl::mainLoop() {
    while (SDL_PollEvent(&Sdl::event)) switch (Sdl::event.type) {
        case SDL_KEYDOWN: switch (Sdl::event.key.keysym.sym) {
            case SDLK_BACKQUOTE:
                SDL_SetWindowFullscreen(Sdl::window, SDL_WINDOW_FULLSCREEN);
                break;
            case SDLK_ESCAPE:
                SDL_SetWindowFullscreen(Sdl::window, 0);
                break;
        } break;
        case SDL_WINDOWEVENT: switch (Sdl::event.window.event) {
            case SDL_WINDOWEVENT_RESIZED:
                Sdl::windowWidth = Sdl::event.window.data1;
                Sdl::windowHeight = Sdl::event.window.data2;
                Sdl::refreshPresentation();
                break;
        } break;
        case SDL_QUIT:
            std::exit(EXIT_SUCCESS);
            break;
    }
    SDL_Delay(1);
}

void Sdl::exitHandler() {
    if (Sdl::window) SDL_DestroyWindow(Sdl::window);
    if (Sdl::renderer) SDL_DestroyRenderer(Sdl::renderer);
    SDL_Quit();
}
