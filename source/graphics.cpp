#include "graphics.hpp"

using namespace Project;

SDL_Window *Sdl::window = nullptr;
SDL_Renderer *Sdl::renderer = nullptr;
int Sdl::windowWidth = 420;
int Sdl::windowHeight = 420;
SDL_Event Sdl::event;

void Sdl::Color::SetRenderDrawColor() const { SDL_SetRenderDrawColor(renderer, red, green, blue, alpha); }

SquareGrid Sdl::grid;
Vector2::HashMap<Sdl::Color> Sdl::colorMap;

void Sdl::renderSquareGrid(SquareGrid const &grid, Vector2::HashMap<Color> const &colorMap) {
    static constexpr Sdl::Color wallColor{0x20, 0x20, 0x95, SDL_ALPHA_OPAQUE};
    static constexpr Sdl::Color defaultColor = wallColor.withGreen(wallColor.green * 5);

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

void Sdl::drawHexagon(float const size, SDL_FPoint const &center) {
    Sdl::drawHexagon(center, std::sqrt(3) * size, 2 * size);
}

#include <iostream>

void Sdl::drawHexagon(SDL_FPoint const &center, float const width, float const height) {
    float const halfWidth = width / 2;
    float const halfHeight = height / 2;
    float const quarterHeight = height / 4;

    SDL_FPoint const topPoint{center.x, center.y - halfHeight};
    SDL_FPoint const topLeftPoint{center.x - halfWidth, center.y - quarterHeight};
    SDL_FPoint const topRightPoint{center.x + halfWidth, center.y - quarterHeight};

    SDL_FPoint const bottomPoint{center.x, center.y + halfHeight};
    SDL_FPoint const bottomLeftPoint{center.x - halfWidth, center.y + quarterHeight};
    SDL_FPoint const bottomRightPoint{center.x + halfWidth, center.y + quarterHeight}; 

    std::vector<SDL_Vertex> const vertexList = {
        // Top triangle.
        {topPoint        , SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {topLeftPoint    , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {topRightPoint   , SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},

        // Middle triangles, (rectangle).
        {topLeftPoint    , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {topRightPoint   , SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {bottomLeftPoint , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {topRightPoint   , SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {bottomLeftPoint , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {bottomRightPoint, SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},

        // Bottom triangle.
        {bottomPoint     , SDL_Color{255, 0, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {bottomLeftPoint , SDL_Color{0, 0, 255, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
        {bottomRightPoint, SDL_Color{0, 255, 0, SDL_ALPHA_OPAQUE}, SDL_FPoint{0}},
   };

    int const result = SDL_RenderGeometry(Sdl::renderer, nullptr, vertexList.data(), vertexList.size(), nullptr, 0);
}

void Sdl::refreshPresentation() {
    Sdl::BLACK.SetRenderDrawColor();
    SDL_RenderClear(Sdl::renderer);

    // Sdl::renderSquareGrid();
    
    SDL_FPoint const center{
        static_cast<float>(Sdl::windowWidth) / 2.0f,
        static_cast<float>(Sdl::windowHeight) / 2.0f
    };

    /* render hexagon grid */ {

        float hexagonWidth = 2.0f * (static_cast<float>(Sdl::windowWidth) / 5.0f);
        float hexagonHeight = 4.0f * (static_cast<float>(Sdl::windowHeight) / 7.0f);

        SDL_FPoint row0StartCenter{hexagonWidth, hexagonHeight / 2.0f};
        SDL_FPoint row1StartCenter{hexagonWidth / 2, hexagonHeight + hexagonHeight / 4.0f};

        Sdl::drawHexagon(row0StartCenter, hexagonWidth, hexagonHeight);
        Sdl::drawHexagon(row1StartCenter, hexagonWidth, hexagonHeight);
        Sdl::drawHexagon(SDL_FPoint{row0StartCenter.x + hexagonWidth, row0StartCenter.y}, hexagonWidth, hexagonHeight);
        Sdl::drawHexagon(SDL_FPoint{row1StartCenter.x + hexagonWidth, row1StartCenter.y}, hexagonWidth, hexagonHeight);
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
