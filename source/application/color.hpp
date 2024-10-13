#ifndef Application_Color_hpp
#define Application_Color_hpp

#include "simple_directmedia_layer.hpp"
#include "linear_interpolation.hpp"
#include "maze_engine/vector2.hpp"
#include "maze_engine/maze_generation_iterator.hpp"

#include <tuple>
#include <string>
#include <cassert>

namespace App {
	using ColorTriplet = std::tuple<SDL_Color, SDL_Color, SDL_Color>;

	using TileColorTripletGetter = std::function<ColorTriplet(MazeEngine::Vector2 const &)>;
	using WallColorTripletGetter = std::function<ColorTriplet(MazeEngine::MazeGenerationIterator::Wall const &)>;

	inline constexpr SDL_Color
		websiteBackgroundColor{
			/* red   */ 174u,
			/* green */ 198u,
			/* blue  */ 207u,
			/* alpha */ 0xFFu
		},
		black{0x00, 0x00, 0x00, 0xFF};

	std::string toString(SDL_Color const &color);

	SDL_Color makeRgbaColor(
		double const hue,
		double const saturation=1.0,
		double const luminance=0.5,
		double const alpha=1.0
	);

	struct HslaColor;
}

struct App::HslaColor {

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

	constexpr HslaColor & setHue       (double const paramHue       ) { hue        = paramHue       ; return *this; }
	constexpr HslaColor & setSaturation(double const paramSaturation) { saturation = paramSaturation; return *this; }
	constexpr HslaColor & setLuminance (double const paramLuminance ) { luminance  = paramLuminance ; return *this; }
	constexpr HslaColor & setAlpha     (double const paramAlpha     ) { alpha      = paramAlpha     ; return *this; }

	static double getCyclicHue(
		double const hue,
		double const percentage,
		double const depth
	);

	static double hueWrap(double const value);

	ColorTriplet getColorTriplet(double const percentage, double const colorDepth) const;

	SDL_Color toRgbaColor() const;
	SDL_Color toRgbaColor(double const overrideHue) const;
	void addHue(double const hueSupplement);
	std::string toString() const;
};

#endif
