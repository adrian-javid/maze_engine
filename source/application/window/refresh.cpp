#include "application/window.hpp"

#include "application/delta_time.hpp"
#include "application/linear_interpolation.hpp"
#include "application/performer.hpp"

namespace App::Window {
	static double cyclicPercentage{/* start at zero percent */ 0.0};

	/*
		Color triplet getter based on the cyclic percentage.
	*/
	[[nodiscard]]
	static ColorTriplet getColorTriplet(
		HslaColor tileColor,
		std::optional<double> const luminanceOpt=std::nullopt
	) {
		static constexpr auto getCyclicHue([](double const hue, double const percentageAddend) -> double {
			static constexpr double hueDepth{55.0};
			return HslaColor::getCyclicHue(hue, MazeEngine::Aux::percentageWrap(cyclicPercentage + percentageAddend), hueDepth);
		});
		double const luminance{luminanceOpt.value_or(tileColor.getLuminance())};
		return std::make_tuple(
			tileColor.setLuminance(luminance).toRgbaColor(getCyclicHue(tileColor.getHue(), -.00)),
			tileColor.setLuminance(luminance).toRgbaColor(getCyclicHue(tileColor.getHue(), -.10)),
			tileColor.setLuminance(luminance).toRgbaColor(getCyclicHue(tileColor.getHue(), -.20))
		);
	}

}

void App::Window::refresh() {
	assert(performer.has_value());

	static constexpr HslaColor
		startEndColor    (020.0),
		pathTileColor    (090.0),
		wallColor        (225.0),
		markedTileColor  (300.0),
		unmarkedTileColor(155.0);

	double const deltaPercentage{static_cast<double>(App::getDeltaTime()) * 0.00011};

	cyclicPercentage = MazeEngine::Aux::percentageWrap(cyclicPercentage + deltaPercentage);
	assert(cyclicPercentage >= 0.0);
	assert(cyclicPercentage < 1.0);

	/*
		Color triplets from the color getter.
	*/
	ColorTriplet const
		pathTileColorTriplet    {getColorTriplet(pathTileColor    )},
		wallColorTriplet        {getColorTriplet(wallColor        )},
		markedTileColorTriplet  {getColorTriplet(markedTileColor  )},
		unmarkedTileColorTriplet{getColorTriplet(unmarkedTileColor)},
		startEndColorTriplet    {getColorTriplet(startEndColor    )};

	/*
		Tile hue getter 
	*/
	static constexpr auto tileHueGetter([](
		MazeEngine::Vector2 const tileKey
	) -> decltype(HslaColor({}).getHue()) {
		assert(performer.has_value());
		if (not performer.has_value()) return {};

		auto const &identities{performer->getMazeGenerationIterator().getTileKeyIdentities()};
		
		auto const identityPair(identities.find(tileKey));
		if (identityPair == identities.cend()) return {};

		MazeEngine::UnionFinder::Identifier const tileKeyIdentifier{identityPair->second};

		return HslaColor::hueWrap(performer->getUnionFinderView().find(tileKeyIdentifier) * 7);
	});

	auto const tileColorTripletGetter([
		&markedTileColorTriplet, &unmarkedTileColorTriplet, &pathTileColorTriplet,
		&startEndColorTriplet
	](MazeEngine::Vector2 const &tileKey) -> ColorTriplet {
		switch (performer->getState()) {
			case Performer::State::generating: {
				return getColorTriplet(tileHueGetter(tileKey));
			}
			default: {
				if (tileKey == performer->getMazeStart() or tileKey == performer->getMazeEnd())
					return startEndColorTriplet;

				if (
					auto const &pathTiles{performer->getPathTileSet()};
					pathTiles.find(tileKey) != pathTiles.end()
				)
					return pathTileColorTriplet;
				else if (
					auto const &markedTiles{performer->getMarkedTileSet()};
					markedTiles.find(tileKey) != markedTiles.end()
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

				static_assert(wallColor.getHue() >= unmarkedTileColor.getHue());
				double const wallTileColorOffset{wallColor.getHue() - unmarkedTileColor.getHue()};
				double const wallHue{HslaColor::hueWrap(tileHueGetter(wall.tileKey) + wallTileColorOffset)};

				if (
					auto const &markedWalls{performer->getMarkedWallSet()};
					markedWalls.find(principalWall) != markedWalls.cend()
				) {
					return getColorTriplet(wallHue);
				} else {
					return getColorTriplet(wallHue, /* luminance */double{0.10});
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

	float const windowWidthFloat {static_cast<float>(windowWidth )};
	float const windowHeightFloat{static_cast<float>(windowHeight)};

	std::visit(
		[
			windowWidthFloat, windowHeightFloat,
			&tileColorTripletGetter, &wallColorTripletGetter
		](auto const &maze) -> void {
			using MazeT = std::decay_t<decltype(maze)>;

			if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>) drawSquareMaze(
				std::forward<decltype(maze)>(maze),
				{0.0f, 0.0f},
				windowWidthFloat, windowHeightFloat,
				tileColorTripletGetter, wallColorTripletGetter
			); else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>) drawHexagonMaze(
				std::forward<decltype(maze)>(maze),
				{windowWidthFloat / 2.0f, windowHeightFloat / 2.0f},
				windowWidthFloat, windowHeightFloat,
				tileColorTripletGetter, wallColorTripletGetter
			);
		},
		performer->getUnderlyingMaze()
	);

	SDL_RenderPresent(renderer);
}
