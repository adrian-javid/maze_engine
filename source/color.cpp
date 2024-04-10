#include "color.hpp"

#include <sstream>
#include <cassert>

using namespace Project;

std::string Media::toString(SDL_Color const &color) {
    std::stringstream buffer;
    buffer << "(R=" << +color.r << ", G=" << +color.g << ", B=" << +color.b << ", A=" << +color.a << ")";
    return buffer.str();
}

SDL_Color Media::HslaColor::toRgbaColor(double hueSupplement) const {
    double const hueValue = HslaColor::wrapHue(this->hue + hueSupplement);

    assert(0 <= hueValue and hueValue < 360);
    assert(0 <= saturation and saturation <= 1);
    assert(0 <= luminance and saturation <= 1);
    assert(0 <= alpha and alpha <= 1);

    auto const chroma = (1.0 - std::fabs(2.0 * luminance - 1.0)) * saturation;

    auto const X = chroma * (1.0 - std::fabs(std::fmod(hueValue / 60.0, 2.0) - 1.0));

    struct Rgb { double r, g, b; } color{};

    /**/ if (  0 <= hueValue and hueValue <  60) color = Rgb{chroma,      X,    0.0}; 
    else if ( 60 <= hueValue and hueValue < 120) color = Rgb{     X, chroma,    0.0};
    else if (120 <= hueValue and hueValue < 180) color = Rgb{   0.0, chroma,      X};
    else if (180 <= hueValue and hueValue < 240) color = Rgb{   0.0,      X, chroma};
    else if (240 <= hueValue and hueValue < 300) color = Rgb{     X,    0.0, chroma};
    else if (300 <= hueValue and hueValue < 360) color = Rgb{chroma,    0.0,      X};

    auto const m = luminance - chroma / 2.0;

    return {
        static_cast<Uint8>((color.r + m) * 0xFF),
        static_cast<Uint8>((color.g + m) * 0xFF),
        static_cast<Uint8>((color.b + m) * 0xFF),
        static_cast<Uint8>((alpha      ) * 0xFF),
    };
}

double Media::HslaColor::wrapHue(double hue, double const bound) {
    hue = std::fmod(hue, bound);
    if (hue < 0) hue += bound;
    if (hue >= bound) hue = 0.0;
    return hue;
}

void Media::HslaColor::addHue(double const hueSupplement) {
    hue = wrapHue(hue + hueSupplement);
}

std::string Media::HslaColor::toString() const {
    std::stringstream buffer;
    buffer << "(H=" << hue << ", S=" << saturation << ", L=" << luminance << ", A=" << alpha << ")";
    return buffer.str();
}
