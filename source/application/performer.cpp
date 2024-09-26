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
		.value("GREEDY" , App::Performer::SearchType::greedy );

	emscripten::enum_<App::Performer::SoundType>("MazeEngine_SoundType")
		.value("NONE"       , App::Performer::SoundType::none       )
		.value("PIANO"      , App::Performer::SoundType::piano      )
		.value("SYNTHESIZER", App::Performer::SoundType::synthesizer);

	emscripten::function("MazeEngine_resetPerformer", +[](
		App::Performer::MazeType const mazeType, int const mazeSize,
		unsigned int const seed, bool const mazeWrap,
		App::Performer::SearchType const searchType,
		App::Performer::SoundType const soundType,
		unsigned int const sleepTimeMilliseconds
	) -> void {
		App::performer.emplace(
			mazeType, mazeSize,
			App::Performer::Seed{seed}, mazeWrap,
			searchType, soundType,
			App::UnsignedMilliseconds{sleepTimeMilliseconds}
		);
	});

}

#endif

App::Performer::Performer(
	MazeType const mazeType, int const mazeSizeHint,
	Seed const seed, bool const mazeWrap,
	SearchType const searchType,
	SoundType const soundType,
	UnsignedMilliseconds const sleepTimeMilliseconds
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
				// Doesn't count center hexagon as part of the radius.
				int const hexagonRadius{mazeSize >= 1 ? mazeSize - 1 : 0};
				return MazeEngine::HexagonMaze(hexagonRadius, mazeFillValue);
			}
		}
	}()),
	mazeStart(std::visit([](auto and(maze)) -> MazeEngine::Vector2 {
		using MazeT = std::decay_t<decltype(maze)>;

		if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>)
			return {0, 0};
		else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>)
			return {0, -(maze.getRadius())};
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
				return {0, maze.getRadius()};
		}
	}, mazeVariant)),
	mazeSearchIteratorVariant([this, searchType, seed, mazeWrap]() -> decltype(Performer::mazeSearchIteratorVariant) {
		/*
			The maze starts out fully walled.

			If the maze search iterator is constructed with a maze
			that is fully walled, then it will get zero neighbors when it attempts
			to get neighbors of the maze start vertex on construction.
			So, it will be an ended iterator to begin with, which is not ideal.

			That is why, here, the maze needs to be generated before the
			maze search iterator is constructed.
		*/
		for (
			MazeEngine::MazeGenerationIterator iterator(getMaze(), seed, mazeWrap);
			not iterator.isDone();
			iterator.advance()
		);

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
		}
	}()),
	timer(sleepTimeMilliseconds),
	soundInstrument{<:soundType:>() -> SoundTable const * {
		switch (soundType) {
			case SoundType::none:
				return nullptr;

			case SoundType::piano:
				return &piano;

			default:
				std::cerr << "Invalid sound type: " << MazeEngine::Aux::Enum::asInt(soundType) << '.' << '\n';
				assert(false);
				[[fallthrough]];

			case SoundType::synthesizer:
				return &synthesizer;
		}
	}()}
{
	assert(not mazeVariant.valueless_by_exception());
	assert(not mazeSearchIteratorVariant.valueless_by_exception());
}

void App::Performer::playSound(MazeEngine::Vector2 const mainVertex) {
	if (soundInstrument == nullptr) return;

	/*
		Get the hashmap that has the information that is the parent vertex of each
		explored vertex so far.
	*/
	auto const &history{getMazeSearchIterator().getHistory()};

	/*
		Getting the edge that has the main vertex of interest and its parent vertex.
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
	MazeEngine::Vector2 const &parentVertex{edge->/* parent vertex */second};

	/*
		This offset vector represents the direciton in a vector form.

		We need to calculate the enumerated direction from this.
	*/
	MazeEngine::Vector2 const offsetVector(/* destination */mainVertex - /* origin */parentVertex);

	/*
		We will determine which sound to play based on the offset vector.
	*/
	std::visit(<:this, offsetVector:>(auto and(maze)) -> void {
		// Get the maze type.
		using MazeT = std::decay_t<decltype(maze)>;
		
		// Alias for the maze direction enum.
		using Direction = MazeEngine::Maze::Direction;

		/*
			Figure out whether the offset vector is a vector that we can recognise.

			That is, its values should be integers that are either `-1`, `0`, or `+1`.
		*/
		bool const isSimpleOffsetVector{<:offsetVector:>() constexpr -> bool {
			switch (offsetVector.value1) {
				case -1:
				case  0:
				case +1: switch (offsetVector.value2) {
					case -1:
					case  0:
					case +1: return true;
				}
			}
			return false;
		}()};

		/*
			The offset vectors that are able to be recognized are determined
			by which maze grid type is being used.
		*/
		if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>) {
			Direction const direction{<:offsetVector, isSimpleOffsetVector:>() -> MazeEngine::Maze::Direction {
				static constexpr std::array<std::array<Direction, 3u>, 3u> directionMatrix{
					/*    */                      /* -1 */         /*  0 */          /* +1 */
					/* -1 */ std::array{ Direction:: none, Direction::west, Direction:: none, },
					/*  0 */ std::array{ Direction::north, Direction::none, Direction::south, },
					/* +1 */ std::array{ Direction:: none, Direction::east, Direction:: none, },
				};

				if (isSimpleOffsetVector)
					return directionMatrix[offsetVector.value1 + 1][offsetVector.value2 + 1];
				else
					return Direction::none;
			}()};

			switch (direction) {
				case MazeEngine::Maze::Direction::north: soundInstrument->play(3u); break;
				case MazeEngine::Maze::Direction::east : soundInstrument->play(1u); break;
				case MazeEngine::Maze::Direction::south: soundInstrument->play(0u); break;
				case MazeEngine::Maze::Direction::west : soundInstrument->play(2u); break;
				default: break;
			}
		} else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>) {
			Direction const direction{<:offsetVector, isSimpleOffsetVector:>() -> MazeEngine::Maze::Direction {
				static constexpr std::array<std::array<Direction, 3u>, 3u> directionMatrix{
					/*    */                          /* -1 */         /*  0 */              /* +1 */
					/* -1 */ std::array{ Direction::     none, Direction::west, Direction::southwest, },
					/*  0 */ std::array{ Direction::northwest, Direction::none, Direction::southeast, },
					/* +1 */ std::array{ Direction::northeast, Direction::east, Direction::     none, },
				};

				if (isSimpleOffsetVector)
					return directionMatrix[offsetVector.value1 + 1][offsetVector.value2 + 1];
				else
					return Direction::none;
			}()};

			switch (direction) {
				case MazeEngine::Maze::Direction::northeast: soundInstrument->play(4u); break;
				case MazeEngine::Maze::Direction::     east: soundInstrument->play(2u); break;
				case MazeEngine::Maze::Direction::southeast: soundInstrument->play(0u); break;
				case MazeEngine::Maze::Direction::southwest: soundInstrument->play(1u); break;
				case MazeEngine::Maze::Direction::     west: soundInstrument->play(3u); break;
				case MazeEngine::Maze::Direction::northwest: soundInstrument->play(5u); break;
				default: break;
			}
		}
	}, mazeVariant);

}

void App::Performer::update() {
	if (timer.update()) switch (state) {
		case State::searching: {
			if (getMazeSearchIterator().isEnd()) goto switchToBacktracking;

			/* process vertex */ {
				MazeEngine::Vector2 const &vertex{*getMazeSearchIterator()};

				playSound(vertex);

				markedTileSet.insert(MazeEngine::Vector2(vertex));

				if (vertex == mazeEnd) goto switchToBacktracking;
			}

			++getMazeSearchIterator();

			return;
		}

		switchToBacktracking: {
			std::cout << "Explored count: " << markedTileSet.size() << '\n';
			trailEdge = getMazeSearchIterator().getHistory().find(mazeEnd);
			state = State::backtracking;

			[[fallthrough]];
		}

		case State::backtracking: {
			MazeEngine::Vector2::HashMap<MazeEngine::Vector2> const &history{getMazeSearchIterator().getHistory()};

			if (trailEdge->/* child vertex */first == mazeStart) goto switchToComplete;

			/* process edge */ {
				pathTileSet.insert(trailEdge->/* child vertex */first);
				playSound(trailEdge->/* child vertex */first);
			}

			trailEdge = history.find(trailEdge->/* parent vertex */second);

			return;
		}

		switchToComplete: {
			pathTileSet.insert(mazeStart); // include corner
			std::cout << "Path length: " << pathTileSet.size() << '\n';
			state = State::complete;

			[[fallthrough]];
		}

		case State::complete: return;
	}
}
