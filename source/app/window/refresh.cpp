#include "app/window.hpp"

#include "app/application.hpp"
#include "app/util.hpp"

void App::Window::refresh() {

	static constexpr HslaColor
		startEndColor    (91.0, 1.0, 0.49),
		pathTileColor    (0.0            ),
		wallColor        (240.0          ),
		markedTileColor  (300.0          ),
		unmarkedTileColor(155.0          );

	static constexpr double zeroPercent{0.0};
	static double percentage{zeroPercent};
	double const deltaPercentage{static_cast<double>(App::deltaTime) * 0.00011};

	percentage = Util::percentageWrap(percentage + deltaPercentage);
	assert(percentage >= 0.0); assert(percentage < 1.0);

	static constexpr double hueDepth{45.0 + 5.0 + 5.0};
	static constexpr auto getColorTriplet = [](HslaColor const &tileColor) -> ColorTriplet {
		static constexpr auto getCyclicHue = [](double const hue, double const percentageAddend) -> double {
			return HslaColor::getCyclicHue(hue, Util::percentageWrap(percentage + percentageAddend), hueDepth);
		};
		return std::make_tuple(
			tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.00)),
			tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.10)),
			tileColor.toRgbaColor(getCyclicHue(tileColor.hue, -.20))
		);
	};

	ColorTriplet const
		pathTileColorTriplet     = getColorTriplet(pathTileColor    ),
		wallColorTriplet         = getColorTriplet(wallColor        ),
		markedTileColorTriplet   = getColorTriplet(markedTileColor  ),
		unmarkedTileColorTriplet = getColorTriplet(unmarkedTileColor),
		startEndColorTriplet     = getColorTriplet(startEndColor    );

	float const windowWidthValue  = static_cast<float>(windowWidth );
	float const windowHeightValue = static_cast<float>(windowHeight);

	auto const mainColorGetter = [
		&markedTileColorTriplet, &unmarkedTileColorTriplet, &pathTileColorTriplet,
		&startEndColorTriplet
	](MazeEngine::Vector2 const &key) -> ColorTriplet {
		if (key == App::mazeStart or key == App::mazeEnd)
			return startEndColorTriplet;

		std::lock_guard const lock(tileInfoMutex);
		if (App::pathTileSet.find(key) != App::pathTileSet.end())
			return pathTileColorTriplet;
		else if (App::markedTileSet.find(key) != App::markedTileSet.end())
			return markedTileColorTriplet;
		else
			return unmarkedTileColorTriplet;
	};

	SDL_RenderClear(renderer);

	if (App::maze == &App::squareMaze) drawSquareMaze(
		squareMaze,
		{0.0f, 0.0f},
		windowWidthValue, windowHeightValue,
		mainColorGetter, wallColorTriplet
	); else if (App::maze == &App::hexagonMaze) drawHexagonMaze(
		hexagonMaze,
		{windowWidthValue / 2.0f, windowHeightValue / 2.0f},
		windowWidthValue, windowHeightValue,
		mainColorGetter, wallColorTriplet
	); else
		throw App::maze;

	SDL_RenderPresent(renderer);
}
