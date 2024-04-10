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

    SDL_Color makeRgbaColor(
        double const hue,
        double const saturation=1.0,
        double const luminance=0.5,
        double const alpha=1.0
    );

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

        constexpr auto hueWrap = [](double const value) constexpr -> double {
            constexpr double fullCycle{360.0};
            return Util::wrapValue(value, fullCycle);
        };

        double const hueValue = value < endValue ? hueWrap(value) :
        // hueWrap(endValue - (value - endValue));
        56.0;

        return {
            toRgbaColor(hueValue),
            toRgbaColor(hueWrap(hueValue - 20.0)),
            toRgbaColor(hueWrap(hueValue - 40.0))
        };
    }

    SDL_Color toRgbaColor() const;
    SDL_Color toRgbaColor(double const overrideHue) const;
    static double wrapHue(double hue, double const bound=360.0);
    void addHue(double const hueSupplement);
    std::string toString() const;
};

#endif
