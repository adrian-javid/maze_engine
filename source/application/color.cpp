#include "color.hpp"

#include <cassert>
#include "maze_engine/auxiliary.hpp"



SDL_Color App::makeRgbaColor(
	double const hue,
	double const saturation,
	double const luminance,
	double const alpha
) {
	assert(0 <= hue and hue < 360);
	assert(0 <= saturation and saturation <= 1);
	assert(0 <= luminance and luminance <= 1);
	assert(0 <= alpha and alpha <= 1);

	double const chroma{(1.0 - std::fabs(2.0 * luminance - 1.0)) * saturation};

	double const X{chroma * (1.0 - std::fabs(std::fmod(hue / 60.0, 2.0) - 1.0))};

	struct Rgb { double r, g, b; } color{};

	/**/ if (  0 <= hue and hue <  60) color = Rgb{chroma,      X,    0.0}; 
	else if ( 60 <= hue and hue < 120) color = Rgb{     X, chroma,    0.0};
	else if (120 <= hue and hue < 180) color = Rgb{   0.0, chroma,      X};
	else if (180 <= hue and hue < 240) color = Rgb{   0.0,      X, chroma};
	else if (240 <= hue and hue < 300) color = Rgb{     X,    0.0, chroma};
	else if (300 <= hue and hue < 360) color = Rgb{chroma,    0.0,      X};

	double const m{luminance - chroma / 2.0};

	return {
		static_cast<Uint8>((color.r + m) * 0xFF),
		static_cast<Uint8>((color.g + m) * 0xFF),
		static_cast<Uint8>((color.b + m) * 0xFF),
		static_cast<Uint8>((alpha      ) * 0xFF),
	};
}

double App::HslaColor::hueWrap(double const value) {
	static constexpr double fullCycle{360.0};
	return MazeEngine::Aux::wrap(value, fullCycle);
}

double App::HslaColor::getCyclicHue(
	double const hue,
	double const percentage,
	double const depth
) {
	assert(hue >= 0.0); assert(hue < 360.0);
	double const hueOffset{linearInterpolation(percentage, 0.0, 2.0 * depth)};
	if (hueOffset < depth)
		return hueWrap(hue + hueOffset);
	else
		return hueWrap((hue + depth) - (hueOffset - depth));
}

auto App::HslaColor::getColorTriplet(double const percentage, double const colorDepth) const -> ColorTriplet {
	double const hueOffset{linearInterpolation(percentage, 0.0, 2.0 * colorDepth)};

	static constexpr auto getHueValue([](
		double const hueValue, double const hueOffsetValue, double const colorDepthValue
	) -> double {
		if (hueOffsetValue < colorDepthValue)
			return hueWrap(hueValue + hueOffsetValue);
		else
			return hueWrap((hueValue + colorDepthValue) - (hueOffsetValue - colorDepthValue));
	});

	return {
		toRgbaColor(getHueValue(hue, hueOffset -  0.0, colorDepth)),
		toRgbaColor(getHueValue(hue, hueOffset - 20.0, colorDepth)),
		toRgbaColor(getHueValue(hue, hueOffset - 40.0, colorDepth))
	};
}

SDL_Color App::HslaColor::toRgbaColor() const { return toRgbaColor(this->hue); }

SDL_Color App::HslaColor::toRgbaColor(double const overrideHue) const {
	return makeRgbaColor(overrideHue, this->saturation, this->luminance, this->alpha);
}

void App::HslaColor::addHue(double const hueSupplement) {
	hue = hueWrap(hue + hueSupplement);
	assert(0.0 <= hue and hue < 360.0);
}
