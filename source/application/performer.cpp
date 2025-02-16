#include "performer.hpp"

#include "maze_engine/maze.hpp"
#include "maze_engine/auxiliary.hpp"
#include "maze_engine/maze_generation_iterator.hpp"

#include <iostream>

std::optional<App::Performer> App::performer(std::nullopt);
App::SoundTable App::Performer::piano;
App::SoundTable App::Performer::synthesizer;

#ifdef __EMSCRIPTEN__

#include <emscripten/bind.h>

EMSCRIPTEN_BINDINGS(MazeEngine) {

	emscripten::enum_<App::Performer::MazeType>("MazeEngine_MazeType")
		.value("HEXAGON", App::Performer::MazeType::hexagon)
		.value("SQUARE" , App::Performer::MazeType::square );

	emscripten::enum_<App::Performer::SearchType>("MazeEngine_SearchType")
		.value("DEPTH"  , App::Performer::SearchType::depth  )
		.value("BREADTH", App::Performer::SearchType::breadth)
		.value("GREEDY" , App::Performer::SearchType::greedy )
		.value("A_STAR" , App::Performer::SearchType::aStar  );

	emscripten::enum_<App::Performer::SoundType>("MazeEngine_SoundType")
		.value("NONE"       , App::Performer::SoundType::none       )
		.value("PIANO"      , App::Performer::SoundType::piano      )
		.value("SYNTHESIZER", App::Performer::SoundType::synthesizer);

	emscripten::function("MazeEngine_resetPerformer", +[](
		App::Performer::MazeType const mazeType, int const mazeSize,
		unsigned int const seed, bool const mazeWrap,
		unsigned int const excessWallPruneCountdown,
		App::Performer::SearchType const searchType,
		App::Performer::SoundType const soundType,
		double const baseHueOffset,
		unsigned int const sleepTimeMilliseconds,
		bool const shouldShowMazeGeneration
	) -> void {
		App::performer.emplace(
			mazeType, mazeSize,
			App::Performer::SeedInt{seed}, mazeWrap,
			std::size_t{excessWallPruneCountdown},
			searchType, soundType, App::Performer::HueFloat{baseHueOffset},
			App::UnsignedMilliseconds{sleepTimeMilliseconds},
			shouldShowMazeGeneration
		);
	});

	emscripten::function("MazeEngine_setSoundInstrument", +[](App::Performer::SoundType const soundType) -> void {
		App::performer->setSoundInstrument(soundType);
	});

	emscripten::function("MazeEngine_setBaseHueOffset", +[](App::Performer::HueFloat const baseHueOffset) -> void {
		App::performer->setBaseHueOffset(baseHueOffset);
	});

	emscripten::function("MazeEngine_setTimeUpdateIntervalMilliseconds", +[](unsigned int const intervalMilliseconds) -> void {
		App::performer->setTimeUpdateInterval(intervalMilliseconds);
	});

	emscripten::function("MazeEngine_playSound", +[](unsigned int const index) -> void {
		if (index >= std::tuple_size_v<App::SoundTable::Data>) {
			std::cerr <<
				"Bad sound index `" << index << "` for size `" <<
				std::tuple_size_v<App::SoundTable::Data> << "`.\n";
			return;
		}

		if (not App::performer.has_value()) {
			std::cerr << "Performer is null.\n";
			return;
		}

		if (App::performer->getSoundInstrument() == nullptr) {
			std::cout << "Performer's sound instrument is null.\n";
			return;
		}

		App::performer->getSoundInstrument()->play(index);
	});

}

#endif

App::Performer::Performer(
	MazeType const mazeType, int const mazeSizeHint,
	SeedInt const seed, bool const mazeWrap,
	std::size_t const excessWallPruneCountdown,
	SearchType const searchType,
	SoundType const soundType,
	HueFloat const paramBaseHueOffset,
	UnsignedMilliseconds const sleepTimeMilliseconds,
	bool const showMazeGeneration
):
	mazeVariant([mazeType, mazeSizeHint]() -> decltype(Performer::mazeVariant) {
		static constexpr MazeEngine::Maze::Tile mazeFillValue{0xFFu};
		static constexpr int mazeSizeMin{1}, mazeSizeMax{1 << 6};

		assert(mazeSizeHint >= mazeSizeMin);
		assert(mazeSizeHint <= mazeSizeMax);

		switch (int const mazeSize{std::clamp(mazeSizeHint, mazeSizeMin, mazeSizeMax)}; mazeType) {
			default:
				std::cerr << "Invalid maze type: " << MazeEngine::Aux::Enum::asInt(mazeType) << '.' << '\n';
				assert(false);
				[[fallthrough]];

			case MazeType::square: {
				int const squareLength{2 * mazeSize - 1};
				return MazeEngine::SquareMaze(squareLength, squareLength, mazeFillValue);
			}

			case MazeType::hexagon: {
				MazeEngine::Vector2::Value const hexagonRadius{mazeSize};
				return MazeEngine::HexagonMaze(hexagonRadius, mazeFillValue);
			}
		}
	}()),
	mazeStart(std::visit([](auto and(maze)) -> MazeEngine::Vector2 {
		using MazeT = std::decay_t<decltype(maze)>;

		if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>)
			return {0, 0};
		else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>)
			return {0, -(maze.getOuterRadius())};
	}, mazeVariant )),
	mazeEnd(std::visit([mazeWrap](auto and(maze)) -> MazeEngine::Vector2 {
		using MazeT = std::decay_t<decltype(maze)>;

		if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>) {
			if (mazeWrap)
				return {maze.getRowCount() / 2, maze.getColumnCount() / 2};
			else
				return {maze.getRowCount() - 1, maze.getColumnCount() - 1};
		} else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>) {
			if (mazeWrap)
				return {0, 0};
			else
				return {0, maze.getOuterRadius()};
		}
	}, mazeVariant)),
	mazeGenerationIterator(getMaze(), seed, mazeWrap, excessWallPruneCountdown),
	mazeSearchIteratorVariant([this, searchType]() -> decltype(Performer::mazeSearchIteratorVariant) {
		switch (searchType) {
			case SearchType::depth:
				return MazeEngine::DepthFirstSearchIterator(getMaze(), mazeStart);

			case SearchType::breadth:
				return MazeEngine::BreadthFirstSearchIterator(getMaze(), mazeStart);

			default:
				std::cerr << "Invalid search type: " << MazeEngine::Aux::Enum::asInt(searchType) << '.' << '\n';
				assert(false);
				[[fallthrough]];

			case SearchType::greedy:
				return MazeEngine::GreedyBestFirstSearchIterator(getMaze(), mazeStart, mazeEnd);

			case SearchType::aStar:
				return MazeEngine::AStarSearchIterator(getMaze(), mazeStart, mazeEnd);
		}
	}()),
	timer(sleepTimeMilliseconds),
	soundInstrument{dispatchSoundInstrument(soundType)},
	randomSoundPicker(SoundTable::makeRandomSoundPicker(seed)),
	baseHueOffset{paramBaseHueOffset},
	trailEdge(getMazeSearchIterator().getHistory().cend())
{
	assert(not mazeVariant.valueless_by_exception());
	assert(not mazeSearchIteratorVariant.valueless_by_exception());

	if (showMazeGeneration == false) {
		for (; not mazeGenerationIterator.isDone(); mazeGenerationIterator.advance());
	}
}

void App::Performer::playSound(MazeEngine::Vector2 const mainVertex) const {
	if (soundInstrument == nullptr) return;

	/*
		Get the hashmap that has the information that is the parent vertex of each
		explored vertex so far.
	*/
	auto const &history{getMazeSearchIterator().getHistory()};

	/*
		Get the edge that has the main vertex of interest and its parent vertex.
	*/
	auto const edge(history.find(mainVertex));

	/*
		Assert that the main vertex does indeed have a parent vertex.
	*/
	if (edge == history.end()) {
		assert(false);
		return;
	}

	/*
		The edge should contain the main vertex.
	*/
	assert(edge->/* child vertex */first == mainVertex);

	/*
		Get the parent vertex.

		The parent vertex is the vertex that was explored before the main vertex,
		and it should be adjacent to the main vertex.

		We can use this parent vertex to calculate the direction of the step that had to
		occur to get from the parent vertex to the main vertex.

		The direction is what we will use to determine what sound effect will be played;
		each direction is assigned a sound effect.
	*/
	MazeEngine::Vector2 const parentVertex{edge->/* parent vertex */second};

	/*
		If there was no wraparound, this offset vector represents the direction in a vector form.

		We need to calculate the enumerated direction from this.
	*/
	MazeEngine::Vector2 const offsetVector(/* destination */mainVertex - /* origin */parentVertex);

	/*
		We will determine which sound to play based on the offset vector.
	*/
	std::visit(<:this, parentVertex, mainVertex, offsetVector:>(auto const &maze) -> void {
		// Get the maze type.
		using MazeT = std::decay_t<decltype(maze)>;

		// Alias for the maze direction enum.
		using Direction = MazeEngine::Maze::Direction;

		Direction const direction{[&maze, parentVertex, mainVertex, offsetVector]() constexpr -> Direction {
			Direction possiblySimpleDirection{MazeEngine::Maze::getSimpleDirection<MazeT>(offsetVector)};

			switch (possiblySimpleDirection) {
				case Direction::none: {
					maze.forEachValidDirection([
						&maze, &possiblySimpleDirection, parentVertex, mainVertex
					](
						Direction const validDirection
					) constexpr -> void {
						auto const [neighborVertex, wallFlag]{
							maze.checkAdjacent(parentVertex, validDirection)
						};

						// The main vertex cannot be a neighbor from this direction if there is a wall.
						assert(not (wallFlag == true and mainVertex == neighborVertex));

						if (not wallFlag and mainVertex == neighborVertex) {
							assert(possiblySimpleDirection == Direction::none);
							possiblySimpleDirection = validDirection;
						}

					});
					break;
				}
				default: break;
			}

			return possiblySimpleDirection;
		}()};

		playSound<MazeT>(direction);

	}, mazeVariant);
}

void App::Performer::update() {
	if (timer.update()) switch (state) {
		case State::generating: {
			using Wall = MazeEngine::MazeGenerationIterator::Wall;

			if (mazeGenerationIterator.isDone()) goto switchToSearching;

			/* process wall */ {
				Wall const *const wall{mazeGenerationIterator.getWall()};

				assert(wall != nullptr);

				if (wall == nullptr) {
					// (Fallback behavior if the wall is null. The wall should never be null here.)
					assert(mazeGenerationIterator.isDone());
					goto switchToSearching;
				}

				// play sound
				if (soundInstrument != nullptr) soundInstrument->play(randomSoundPicker.getInt());

				markedWallSet.insert(*wall);
			}

			if constexpr (true) {
				mazeGenerationIterator.advanceUntilUnionOrDone([this](Wall const wall) -> void {
					this->markedWallSet.insert(wall);
				});
			} else {
				mazeGenerationIterator.advance();
			}

			return;
		}

		switchToSearching: {
			std::cout << "Finished generating the maze.\n";

			state = State::searching;

			// Skip presenting the first maze tile.
			for (
				MazeEngine::Vector2 vertex{getMazeSearchIterator().getVector()};
				vertex == mazeStart and not getMazeSearchIterator().isDone();
				getMazeSearchIterator().advance(), vertex = getMazeSearchIterator().getVector()
			) {
				markedTileSet.insert(vertex);
			}

			[[fallthrough]];
		}

		case State::searching: {
			if (getMazeSearchIterator().isDone()) goto switchToBacktracking;

			/* process vertex */ {
				MazeEngine::Vector2 const vertex{getMazeSearchIterator().getVector()};

				markedTileSet.insert(MazeEngine::Vector2(vertex));

				if (vertex == mazeEnd) goto switchToBacktracking;

				playSound(vertex);
			}

			getMazeSearchIterator().advance();

			return;
		}

		switchToBacktracking: {
			std::cout << "Explored count: " << markedTileSet.size() << '\n';
			state = State::backtracking;
			MazeEngine::Vector2::HashMap<MazeEngine::Vector2> const &history{getMazeSearchIterator().getHistory()};
			trailEdge = history.find(mazeEnd);
			if (trailEdge != history.cend()) {
				pathTileSet.insert(trailEdge->/* child vertex */first);
				trailEdge = history.find(trailEdge->/* parent vertex */second);
			}

			[[fallthrough]];
		}

		case State::backtracking: {
			MazeEngine::Vector2::HashMap<MazeEngine::Vector2> const &history{getMazeSearchIterator().getHistory()};

			if (trailEdge == history.cend()) return;

			if (trailEdge->/* child vertex */first == mazeStart) goto switchToCelebrating;

			/* process edge */ {
				pathTileSet.insert(trailEdge->/* child vertex */first);
				playSound(trailEdge->/* child vertex */first);
			}

			trailEdge = history.find(trailEdge->/* parent vertex */second);

			return;
		}

		switchToCelebrating: {
			pathTileSet.insert(mazeStart); // include corner
			std::cout << "Path length: " << pathTileSet.size() << '\n';
			state = State::celebrating;
			timer = Timer(App::UnsignedMilliseconds{90u});

			[[fallthrough]];
		}

		case State::celebrating: {
			if (celebrationSong.isDone()) goto switchToComplete;

			celebrationSong.advance(soundInstrument);

			return;
		}

		switchToComplete: {
			state = State::complete;

			[[fallthrough]];
		}

		case State::complete: return;
	}
}
