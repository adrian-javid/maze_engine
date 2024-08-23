#include "application/window.hpp"

#include "application/common.hpp"
#include "application/util.hpp"
#include "application/performer.hpp"

void App::Window::refresh() {

	static constexpr HslaColor
		startEndColor    ( 91.0, 1.0, 0.49),
		pathTileColor    (  0.0           ),
		wallColor        (240.0           ),
		markedTileColor  (300.0           ),
		unmarkedTileColor(155.0           );

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
		if (key == performer->getMazeStart() or key == performer->getMazeEnd())
			return startEndColorTriplet;

		if (
			auto const &pathTiles = performer->getPathTileSet();
			pathTiles.find(key) != pathTiles.end()
		)
			return pathTileColorTriplet;
		else if (
			auto const &markedTiles = performer->getMarkedTileSet();
			markedTiles.find(key) != markedTiles.end()
		)
			return markedTileColorTriplet;
		else
			return unmarkedTileColorTriplet;
	};

	SDL_RenderClear(renderer);

	std::visit(
		[
			windowWidthValue, windowHeightValue,
			&mainColorGetter, &wallColorTriplet
		](auto &&maze) -> void {
			using MazeT = std::decay_t<decltype(maze)>;

			if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>) drawSquareMaze(
				std::forward<decltype(maze)>(maze),
				{0.0f, 0.0f},
				windowWidthValue, windowHeightValue,
				mainColorGetter, wallColorTriplet
			); else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>) drawHexagonMaze(
				std::forward<decltype(maze)>(maze),
				{windowWidthValue / 2.0f, windowHeightValue / 2.0f},
				windowWidthValue, windowHeightValue,
				mainColorGetter, wallColorTriplet
			);
		},
		performer->getUnderlyingMaze()
	);

	SDL_RenderPresent(renderer);
}
