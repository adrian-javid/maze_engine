#include "performer.hpp"

#include "maze_engine/maze.hpp"

#include <iostream>

std::optional<App::Performer> App::Performer::performer(std::nullopt);

App::Performer::Performer(
	MazeType const mazeType, int const mazeSize,
	unsigned int const seed, bool const mazeWrap,
	SearchType const searchType,
	decltype(Performer::sleepTime) sleepTimeMilliseconds
):
	mazeVariant([mazeType, mazeSize]() -> decltype(Performer::mazeVariant) {
		static constexpr MazeEngine::Maze::Tile mazeFillValue{0xFFu};

		switch (mazeType) {
			case MazeType::square:
				return MazeEngine::SquareMaze(mazeSize, mazeSize, mazeFillValue);
			
			case MazeType::hexagon:
				return MazeEngine::HexagonMaze(mazeSize, mazeFillValue);

			default:
				throw mazeType;
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
	mazeSearchIteratorVariant([this, searchType]() -> decltype(Performer::mazeSearchIteratorVariant) {
		switch (searchType) {
			case SearchType::depth:
				return MazeEngine::DepthFirstSearchIterator(getMaze(), mazeStart);

			case SearchType::breadth:
				return MazeEngine::BreadthFirstSearchIterator(getMaze(), mazeStart);

			case SearchType::greedy:
				return MazeEngine::GreedyBestFirstSearchIterator(getMaze(), mazeStart, mazeEnd);

			default:
				throw searchType;
		}
	}()),
	sleepTime{sleepTimeMilliseconds}
{
	getMaze().generate(seed, mazeWrap);
}

void App::Performer::update() {
	switch (state) {
		case State::searching: {
			if (getMazeSearchIterator().isEnd()) goto switchToBacktracking;

			/* process vertex */ {
				MazeEngine::Vector2 const &vertex(*getMazeSearchIterator());
				markedTileSet.insert(vertex);
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
