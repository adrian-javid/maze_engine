#ifndef Applciation_Performer_hpp
#define Applciation_Performer_hpp

#include <variant>

#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "maze_engine/search/breadth_first.hpp"
#include "maze_engine/search/depth_first.hpp"
#include "maze_engine/search/greedy_best_first.hpp"
#include "maze_engine/search/a_star.hpp"
#include "simple_directmedia_layer.hpp"

namespace App { class Performer; }

class App::Performer {
	public:

		enum struct MazeType : std::uint_least8_t { hexagon = 1u, square };

		enum struct SearchType : std::uint_least8_t { depth = 1u, breadth, greedy };

		enum struct State : std::uint_least8_t { searching = 1u, backtracking, complete };

	private:

		std::variant<MazeEngine::SquareMaze, MazeEngine::HexagonMaze> mazeVariant;
		MazeEngine::Vector2 mazeStart;
		MazeEngine::Vector2 mazeEnd;
		std::variant<
			MazeEngine::DepthFirstSearchIterator,
			MazeEngine::BreadthFirstSearchIterator,
			MazeEngine::GreedyBestFirstSearchIterator
		> mazeSearchIteratorVariant;

		MazeEngine::Vector2::HashSet markedTileSet;
		MazeEngine::Vector2::HashSet pathTileSet;
		/*
			`edge->first` is the child vertex
			`edge->second` is the parent vertex
		*/
		MazeEngine::Vector2::HashMap<MazeEngine::Vector2>::const_iterator edge;
		Uint32 sleepTime{0u};
		State state{State::searching};

	public:

		[[nodiscard]] explicit
		Performer(
			MazeType const mazeType, int const mazeSize,
			unsigned int const seed, bool const mazeWrap,
			SearchType const searchType
		);

		FORCE_INLINE
		MazeEngine::Maze const & getMaze() const {
			return std::visit(
				[](auto &&maze) -> MazeEngine::Maze const & {
					return maze;
				},
				mazeVariant
			);
		}

		FORCE_INLINE
		MazeEngine::Maze & getMaze() {
			return const_cast<MazeEngine::Maze &>(std::as_const(*this).getMaze());
		}

		FORCE_INLINE
		MazeEngine::MazeSearchIterator const & getMazeSearchIterator() const {
			return std::visit(
				[](auto &&mazeSearchIterator) -> MazeEngine::MazeSearchIterator const & {
					return mazeSearchIterator;
				},
				mazeSearchIteratorVariant
			);
		}

		FORCE_INLINE
		MazeEngine::MazeSearchIterator & getMazeSearchIterator() {
			return const_cast<MazeEngine::MazeSearchIterator &>(std::as_const(*this).getMazeSearchIterator());
		}

		void update();
};

#endif
