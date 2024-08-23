#include "performer.hpp"

#include "maze_engine/maze.hpp"
#include "maze_engine/auxiliary.hpp"

#include <iostream>

std::optional<App::Performer> App::performer(std::nullopt);

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

	emscripten::function("MazeEngine_resetPerformer", +[](
		App::Performer::MazeType const mazeType, int const mazeSize,
		App::Performer::Seed const seed, bool const mazeWrap,
		App::Performer::SearchType const searchType,
		App::Performer::Milliseconds const sleepTimeMilliseconds
	) -> void {
		App::performer.emplace(
			mazeType, mazeSize,
			seed, mazeWrap,
			searchType,
			sleepTimeMilliseconds
		);
	});

}

#endif

App::Performer::Performer(
	MazeType const mazeType, int const mazeSizeHint,
	Seed const seed, bool const mazeWrap,
	SearchType const searchType,
	Milliseconds const sleepTimeMilliseconds
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
	mazeStart(std::visit([](auto &&maze) -> MazeEngine::Vector2 {
		using MazeT = std::decay_t<decltype(maze)>;

		if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>)
			return {0, 0};
		else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>)
			return {0, -(maze.getRadius())};
	}, mazeVariant )),
	mazeEnd(std::visit([mazeWrap](auto &&maze) -> MazeEngine::Vector2 {
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
		getMaze().generate(seed, mazeWrap);

		switch (searchType) {
			case SearchType::depth:
				return MazeEngine::DepthFirstSearchIterator(getMaze(), mazeStart);

			case SearchType::breadth:
				return MazeEngine::BreadthFirstSearchIterator(getMaze(), mazeStart);

			default:
				std::cerr << "Invalid search type: " << MazeEngine::Aux::Enum::asInt(searchType) << '.' << '\n';
				assert(false);
				[[fallthrough]]

			case SearchType::greedy:
				return MazeEngine::GreedyBestFirstSearchIterator(getMaze(), mazeStart, mazeEnd);
		}
	}()),
	sleepTime{sleepTimeMilliseconds}
{
	assert(not mazeVariant.valueless_by_exception());
	assert(not mazeSearchIteratorVariant.valueless_by_exception());
}

void App::Performer::update() {
	switch (state) {
		case State::searching: {
			if (getMazeSearchIterator().isEnd()) goto switchToBacktracking;

			/* process vertex */ {
				MazeEngine::Vector2 const &vertex(*getMazeSearchIterator());
				
				markedTileSet.insert(MazeEngine::Vector2(vertex));
				SDL_Delay(sleepTime);

				if (vertex == mazeEnd) goto switchToBacktracking;
			}

			++getMazeSearchIterator();

			return;
		}

		switchToBacktracking: {
			std::cout << "Explored count: " << markedTileSet.size() << '\n';
			edge = getMazeSearchIterator().getHistory().find(mazeEnd);
			state = State::backtracking;

			[[fallthrough]];
		}

		case State::backtracking: {
			auto const &history = getMazeSearchIterator().getHistory();

			if (edge->/* child vertex */first == mazeStart) goto switchToComplete;

			/* process edge */ {
				pathTileSet.insert(edge->/* child vertex */first);
				SDL_Delay(sleepTime);
			}

			edge = history.find(edge->/* parent vertex */second);
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
