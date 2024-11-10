#include "application/window.hpp"

#include "application/delta_time.hpp"
#include "application/linear_interpolation.hpp"
#include "application/performer.hpp"

namespace App::Window {
	static double cyclicPercentage{/* start at zero percent */ 0.0};

	using HueFloat = decltype(std::declval<HslaColor>().getHue());

	namespace BaseHue {
		static constexpr HueFloat
			unmarkedTile(HslaColor(155.0).getHue()),
			  markedTile(HslaColor(300.0).getHue()),
			    startEnd(HslaColor(100.0).getHue()),
			    pathTile(HslaColor(026.0).getHue()),
			        wall(HslaColor(225.0).getHue());

		static constexpr HueFloat
			markedTileOffset(markedTile - unmarkedTile),
			  startEndOffset(  startEnd - unmarkedTile),
			  pathTileOffset(  pathTile - unmarkedTile),
			      wallOffset(      wall - unmarkedTile);
	}

	struct ColorScheme {
		[[nodiscard]]
		explicit ColorScheme(double const baseHue):
			unmarkedTileHue{baseHue},
			markedTileHue{HslaColor::hueWrap(baseHue + BaseHue::markedTileOffset)},
			startEndHue  {HslaColor::hueWrap(baseHue + BaseHue::  startEndOffset)},
			pathTileHue  {HslaColor::hueWrap(baseHue + BaseHue::  pathTileOffset)},
			wallHue      {HslaColor::hueWrap(baseHue + BaseHue::      wallOffset)}
		{}

		/*
			Make a color scheme based on the tile identities from the maze generation iterator.
		*/
		[[nodiscard]]
		static ColorScheme make(MazeEngine::Vector2 const tileKey);

		public: HueFloat
			unmarkedTileHue{},
			  markedTileHue{},
			    startEndHue{},
			    pathTileHue{},
			        wallHue{};
	};

	/*
		Color scheme with the legacy colors.
	*/
	static ColorScheme const legacyColorScheme(BaseHue::unmarkedTile);

	ColorScheme ColorScheme::make(MazeEngine::Vector2 const tileKey) {
		if (not performer.has_value()) {
			assert(false);
			return legacyColorScheme;
		}

		auto const &identities{
			performer->getMazeGenerationIterator().getTileKeyIdentities()
		};

		auto const identityPair(identities.find(tileKey));
		if (identityPair == identities.cend()) {
			assert(false);
			return legacyColorScheme;
		}

		MazeEngine::UnionFinder::Identifier const tileKeyIdentifier{identityPair->second};

		HueFloat const baseHue{
			HslaColor::hueWrap(performer->getUnionFinderView().find(tileKeyIdentifier) * 15 + performer->getBaseHueOffset())
		};

		return ColorScheme(baseHue);
	}

	/*
		Get color triplet based on the cyclic percentage.
	*/
	[[nodiscard]]
	static inline ColorTriplet getColorTriplet(
		HueFloat const tileHue, double const luminance=HslaColor::defaultLuminance
	) {
		static constexpr auto getCyclicHue([](
			double const hue,
			double const percentageAddend
		) -> double {
			static constexpr double hueDepth{49.5};
			return HslaColor::getCyclicHue(
				hue,
				MazeEngine::Aux::percentageWrap(cyclicPercentage + percentageAddend),
				hueDepth
			);
		});
		return std::make_tuple(
			HslaColor(tileHue).setLuminance(luminance).toRgbaColor(getCyclicHue(tileHue, -.00)),
			HslaColor(tileHue).setLuminance(luminance).toRgbaColor(getCyclicHue(tileHue, -.10)),
			HslaColor(tileHue).setLuminance(luminance).toRgbaColor(getCyclicHue(tileHue, -.20))
		);
	}

}

void App::Window::refresh() {
	assert(performer.has_value());

	double const deltaPercentage{static_cast<double>(App::getDeltaTime()) * 0.00011};

	cyclicPercentage = MazeEngine::Aux::percentageWrap(cyclicPercentage + deltaPercentage);
	assert(cyclicPercentage >= 0.0);
	assert(cyclicPercentage <  1.0);

	auto const tileColorTripletGetter([](
		MazeEngine::Vector2 const &tileKey
	) -> ColorTriplet {
		ColorScheme const colorScheme(ColorScheme::make(tileKey));

			if (tileKey == performer->getMazeStart() or tileKey == performer->getMazeEnd())
				return getColorTriplet(colorScheme.startEndHue);

			if (
				auto const &pathTiles{performer->getPathTileSet()};
				pathTiles.find(tileKey) != pathTiles.end()
			)
				return getColorTriplet(colorScheme.pathTileHue);
			else if (
				auto const &markedTiles{performer->getMarkedTileSet()};
				markedTiles.find(tileKey) != markedTiles.end()
			)
				return getColorTriplet(colorScheme.markedTileHue);
			else
				return getColorTriplet(colorScheme.unmarkedTileHue);
	});

	auto const wallColorTripletGetter([](
		MazeEngine::MazeGenerationIterator::Wall const &wall
	) -> ColorTriplet {
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

		ColorScheme const colorScheme(ColorScheme::make(wall.tileKey));

		static_assert(BaseHue::wall >= BaseHue::unmarkedTile);

		assert(performer.has_value());
		switch (performer->getState()) {
			case Performer::State::generating: {
				if (
					auto const &markedWalls{performer->getMarkedWallSet()};
					markedWalls.find(principalWall) != markedWalls.cend()
				) {
					return getColorTriplet(colorScheme.wallHue);
				} else {
					return getColorTriplet(colorScheme.wallHue, /* luminance */double{0.10});
				}
			}
			default: return getColorTriplet(colorScheme.wallHue);
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
