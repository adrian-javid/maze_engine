#include "application/window.hpp"

#include "application/delta_time.hpp"
#include "application/linear_interpolation.hpp"
#include "application/performer.hpp"

namespace App {
	FORCE_INLINE static inline
	double percentageWrap(double const value) {
		return MazeEngine::Aux::wrap(value, 1.00);
	}
}

void App::Window::refresh() {
	assert(performer.has_value());

	static constexpr HslaColor
		startEndColor    ( 91.0, 1.0, 0.49),
		pathTileColor    (  0.0           ),
		wallColor        (240.0           ),
		markedTileColor  (300.0           ),
		unmarkedTileColor(155.0           );

	static constexpr double zeroPercent{0.0};
	static double percentage{zeroPercent};
	double const deltaPercentage{static_cast<double>(App::getDeltaTime()) * 0.00011};

	percentage = App::percentageWrap(percentage + deltaPercentage);
	assert(percentage >= 0.0); assert(percentage < 1.0);

	static constexpr double hueDepth{45.0 + 5.0 + 5.0};
	static constexpr auto getColorTriplet([](
		HslaColor tileColor,
		double const luminance=HslaColor(double{}).luminance
	) -> ColorTriplet {
		static constexpr auto getCyclicHue([](double const hue, double const percentageAddend) -> double {
			return HslaColor::getCyclicHue(hue, App::percentageWrap(percentage + percentageAddend), hueDepth);
		});
		return std::make_tuple(
			tileColor.setLuminance(luminance).toRgbaColor(getCyclicHue(tileColor.hue, -.00)),
			tileColor.setLuminance(luminance).toRgbaColor(getCyclicHue(tileColor.hue, -.10)),
			tileColor.setLuminance(luminance).toRgbaColor(getCyclicHue(tileColor.hue, -.20))
		);
	});

	ColorTriplet const
		pathTileColorTriplet    {getColorTriplet(pathTileColor    )},
		wallColorTriplet        {getColorTriplet(wallColor        )},
		markedTileColorTriplet  {getColorTriplet(markedTileColor  )},
		unmarkedTileColorTriplet{getColorTriplet(unmarkedTileColor)},
		startEndColorTriplet    {getColorTriplet(startEndColor    )};

	float const windowWidthValue {static_cast<float>(windowWidth )};
	float const windowHeightValue{static_cast<float>(windowHeight)};

	auto const tileColorTripletGetter([
		&markedTileColorTriplet, &unmarkedTileColorTriplet, &pathTileColorTriplet,
		&startEndColorTriplet
	](MazeEngine::Vector2 const &key) -> ColorTriplet {
		switch (performer->getState()) {
			case Performer::State::generating: {
				return {};
			}
			default: {
				if (key == performer->getMazeStart() or key == performer->getMazeEnd())
					return startEndColorTriplet;

				if (
					auto const &pathTiles{performer->getPathTileSet()};
					pathTiles.find(key) != pathTiles.end()
				)
					return pathTileColorTriplet;
				else if (
					auto const &markedTiles{performer->getMarkedTileSet()};
					markedTiles.find(key) != markedTiles.end()
				)
					return markedTileColorTriplet;
				else
					return unmarkedTileColorTriplet;
			}
		}
	});

	auto const wallColorTripletGetter([
		&wallColorTriplet
	](MazeEngine::MazeGenerationIterator::Wall const &wall) -> ColorTriplet {
		switch (performer->getState()) {
			case Performer::State::generating: {
				using Wall = MazeEngine::MazeGenerationIterator::Wall;

				Wall const principalWall([wall]() -> Wall {
					switch (wall.type) {
						// "principal" directions
						case MazeEngine::Maze::north    :
						case MazeEngine::Maze::northeast:
						case MazeEngine::Maze::east     :
						case MazeEngine::Maze::southeast:
							return wall;

						default: return {
							std::as_const(performer)->getMaze().checkAdjacent(wall.tileKey, wall.type).key,
							std::as_const(performer)->getMaze().reverseDirection(wall.type)
						};
					}
				}());

				if (
					auto const &markedWalls{performer->getMarkedWallSet()};
					markedWalls.find(principalWall) != markedWalls.cend()
				) {
					return wallColorTriplet;
				} else {
					return getColorTriplet(wallColor, /* luminance */double{0.10});
				}
			}

			default: return wallColorTriplet;
		}
	});

	#ifdef __EMSCRIPTEN__
	setRenderDrawColor(websiteBackgroundColor);
	#else
	setRenderDrawColor(black);
	#endif
	SDL_RenderClear(renderer);

	std::visit(
		[
			windowWidthValue, windowHeightValue,
			&tileColorTripletGetter, &wallColorTripletGetter
		](auto and(maze)) -> void {
			using MazeT = std::decay_t<decltype(maze)>;

			if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>) drawSquareMaze(
				std::forward<decltype(maze)>(maze),
				{0.0f, 0.0f},
				windowWidthValue, windowHeightValue,
				tileColorTripletGetter, wallColorTripletGetter
			); else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>) drawHexagonMaze(
				std::forward<decltype(maze)>(maze),
				{windowWidthValue / 2.0f, windowHeightValue / 2.0f},
				windowWidthValue, windowHeightValue,
				tileColorTripletGetter, wallColorTripletGetter
			);
		},
		performer->getUnderlyingMaze()
	);

	SDL_RenderPresent(renderer);
}
