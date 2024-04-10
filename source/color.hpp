#ifndef color_hpp
#define color_hpp

#include "simpleDirectmediaLayer.hpp"
#include "Util.hpp"

#include <tuple>
#include <string>
#include <cassert>

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

    inline ColorTriplet getColorTriplet(double const percentage, double const colorLength) const {
        double const startValue{this->hue};
        double const endValue{startValue + colorLength};

        double const value{Util::linearInterpolation(percentage, startValue, startValue + 2 * colorLength)};

        constexpr double fullCycle{360.0};

        double const hueValue =
            value < endValue ?
                Util::wrapValue(value, fullCycle) :
                Util::wrapValue(endValue - (value - endValue), fullCycle);

        return {
            this->toRgbaColor(),
            this->toRgbaColor(-20.0),
            this->toRgbaColor(-40.0)
        };
    }

    SDL_Color toRgbaColor(double hueSupplement=0.0) const;
    static double wrapHue(double hue, double const bound=360.0);
    void addHue(double const hueSupplement);
    std::string toString() const;
};

#endif
