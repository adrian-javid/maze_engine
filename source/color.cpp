#include "color.hpp"

#include <sstream>
#include <cassert>

using namespace Project;

std::string Media::toString(SDL_Color const &color) {
    std::stringstream buffer;
    buffer << "(R=" << +color.r << ", G=" << +color.g << ", B=" << +color.b << ", A=" << +color.a << ")";
    return buffer.str();
}

SDL_Color Media::makeRgbaColor(
    double const hue,
    double const saturation,
    double const luminance,
    double const alpha
) {
    assert(0 <= hue and hue < 360);
    assert(0 <= saturation and saturation <= 1);
    assert(0 <= luminance and luminance <= 1);
    assert(0 <= alpha and alpha <= 1);

    auto const chroma = (1.0 - std::fabs(2.0 * luminance - 1.0)) * saturation;

    auto const X = chroma * (1.0 - std::fabs(std::fmod(hue / 60.0, 2.0) - 1.0));

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
        static_cast<Uint8>((alpha      ) * 0xFF),
    };
}

double Media::HslaColor::hueWrap(double const value) {
    constexpr double fullCycle{360.0};
    return Util::wrapValue(value, fullCycle);
}

double Media::HslaColor::getCyclicHue(
    double const hue,
    double const percentage,
    double const depth
) {
    assert(hue >= 0.0); assert(hue < 360.0);
    double const hueOffset{Util::linearInterpolation(percentage, 0.0, 2.0 * depth)};
    if (hueOffset < depth)
        return hueWrap(hue + hueOffset);
    else
        return hueWrap((hue + depth) - (hueOffset - depth));
}

Media::ColorTriplet Media::HslaColor::getColorTriplet(double const percentage, double const colorDepth) const {
    double const hueOffset{Util::linearInterpolation(percentage, 0.0, 2.0 * colorDepth)};

    constexpr auto getHueValue = [](double const hue, double const hueOffset, double const colorDepth) -> double {
        if (hueOffset < colorDepth)
            return hueWrap(hue + hueOffset);
        else
            return hueWrap((hue + colorDepth) - (hueOffset - colorDepth));
    };

    return {
        toRgbaColor(getHueValue(hue, hueOffset -  0.0, colorDepth)),
        toRgbaColor(getHueValue(hue, hueOffset - 20.0, colorDepth)),
        toRgbaColor(getHueValue(hue, hueOffset - 40.0, colorDepth))
    };
}

SDL_Color Media::HslaColor::toRgbaColor() const { return toRgbaColor(this->hue); }

SDL_Color Media::HslaColor::toRgbaColor(double const overrideHue) const {
    return makeRgbaColor(overrideHue, this->saturation, this->luminance, this->alpha);
}

void Media::HslaColor::addHue(double const hueSupplement) {
    hue = hueWrap(hue + hueSupplement);
}

std::string Media::HslaColor::toString() const {
    std::stringstream buffer;
    buffer << "(H=" << hue << ", S=" << saturation << ", L=" << luminance << ", A=" << alpha << ")";
    return buffer.str();
}
