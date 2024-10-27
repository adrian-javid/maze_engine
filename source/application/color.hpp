#ifndef Application_Color_hpp
#define Application_Color_hpp

#include "simple_directmedia_layer.hpp"
#include "linear_interpolation.hpp"
#include "maze_engine/vector2.hpp"
#include "maze_engine/maze_generation_iterator.hpp"

#include <tuple>
#include <string>
#include <cassert>
#include <iostream>
#include <sstream>

namespace App {
	using ColorTriplet = std::tuple<SDL_Color, SDL_Color, SDL_Color>;

	using TileColorTripletGetter = std::function<ColorTriplet(MazeEngine::Vector2 const &)>;
	using WallColorTripletGetter = std::function<ColorTriplet(MazeEngine::MazeGenerationIterator::Wall const &)>;

	inline constexpr SDL_Color
		websiteBackgroundColor{
			/* red   */  174u,
			/* green */  198u,
			/* blue  */  207u,
			/* alpha */ 0xFFu
		},
		black{0x00, 0x00, 0x00, 0xFF};

	[[nodiscard]]
	std::string toString(SDL_Color const &color);

	[[nodiscard]]
	SDL_Color makeRgbaColor(
		double const hue,
		double const saturation=1.0,
		double const luminance=0.5,
		double const alpha=1.0
	);

	struct HslaColor;
}

struct App::HslaColor {

	[[nodiscard]]
	constexpr HslaColor() = delete;

	double static constexpr defaultSaturation=1.0, defaultLuminance=0.5, defaultAlpha=1.0;

	[[nodiscard]]
	constexpr HslaColor(
		double const paramHue                         ,
		double const paramSaturation=defaultSaturation,
		double const paramLuminance =defaultLuminance ,
		double const paramAlpha     =defaultAlpha
	):
		hue       {paramHue       },
		saturation{paramSaturation},
		luminance {paramLuminance },
		alpha     {paramAlpha     }
	{
		assert(0.0 <= hue        and hue        <  360.0);
		assert(0.0 <= saturation and saturation <=   1.0);
		assert(0.0 <= luminance  and luminance  <=   1.0);
		assert(0.0 <= alpha      and alpha      <=   1.0);
	}

	[[nodiscard]] constexpr double getHue       () const { return hue       ; }
	[[nodiscard]] constexpr double getSaturation() const { return saturation; }
	[[nodiscard]] constexpr double getLuminance () const { return luminance ; }
	[[nodiscard]] constexpr double getAlpha     () const { return alpha     ; }

	constexpr HslaColor & setHue       (double const paramHue       ) { hue        = paramHue       ; return *this; }
	constexpr HslaColor & setSaturation(double const paramSaturation) { saturation = paramSaturation; return *this; }
	constexpr HslaColor & setLuminance (double const paramLuminance ) { luminance  = paramLuminance ; return *this; }
	constexpr HslaColor & setAlpha     (double const paramAlpha     ) { alpha      = paramAlpha     ; return *this; }

	[[nodiscard]]
	static double getCyclicHue(double const hue, double const percentage, double const depth);

	[[nodiscard]]
	static double hueWrap(double const value);

	[[deprecated, nodiscard]]
	ColorTriplet getColorTriplet(double const percentage, double const colorDepth) const;

	[[nodiscard]]
	SDL_Color toRgbaColor() const;
	
	[[nodiscard]]
	SDL_Color toRgbaColor(double const overrideHue) const;

	void addHue(double const hueSupplement);

	[[nodiscard]]
	std::string toString() const;

	private:
		double hue={}, saturation=defaultSaturation, luminance=defaultLuminance, alpha=defaultAlpha;
};

#endif
