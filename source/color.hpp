#ifndef color_hpp
#define color_hpp

#include "simpleDirectmediaLayer.hpp"

#include <tuple>
#include <string>

namespace Project::Media {
    using ColorTriplet = std::tuple<SDL_Color, SDL_Color, SDL_Color>;

    constexpr SDL_Color black{0x00, 0x00, 0x00, 0xFF};

    std::string toString(SDL_Color const &color);

    struct HslaColor;
}

struct Project::Media::HslaColor {
    double hue, saturation, luminance, alpha;
    constexpr HslaColor(
        double const hueValue,
        double const saturationValue=1.0,
        double const luminanceValue=0.5,
        double const alphaValue=1.0
    ):
        hue{hueValue},
        saturation{saturationValue},
        luminance{luminanceValue},
        alpha{alphaValue}
    {}
    SDL_Color toRgbaColor(double hueSupplement=0.0) const;
    static double wrapHue(double hue, double const bound=360.0);
    void addHue(double const hueSupplement);
    std::string toString() const;
};

#endif
