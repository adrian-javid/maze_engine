#ifndef Applciation_Performer_hpp
#define Applciation_Performer_hpp

#include <variant>
#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "maze_engine/search/breadth_first.hpp"
#include "maze_engine/search/depth_first.hpp"
#include "maze_engine/search/greedy_best_first.hpp"
#include "maze_engine/search/a_star.hpp"

namespace App { class Performer; }

class App::Performer {
	public:

		enum struct MazeType : std::uint_least8_t { hexagon = 1u, square };

		enum struct SearchType : std::uint_least8_t { depth = 1u, breadth, greedy };

	private:

		std::variant<MazeEngine::SquareMaze, MazeEngine::HexagonMaze> mazeVariant;
		MazeEngine::Vector2 mazeStart;
		MazeEngine::Vector2 mazeEnd;
		std::variant<
			MazeEngine::DepthFirstSearchIterator,
			MazeEngine::BreadthFirstSearchIterator,
			MazeEngine::GreedyBestFirstSearchIterator
		> mazeSearchIteratorVariant;

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

		void update();

	private:

	std::size_t exploredVertexCount{0u};

};

#endif
