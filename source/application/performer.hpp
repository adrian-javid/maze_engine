#ifndef Application_Performer_hpp
#define Application_Performer_hpp

#include <variant>

#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "maze_engine/search/breadth_first.hpp"
#include "maze_engine/search/depth_first.hpp"
#include "maze_engine/search/greedy_best_first.hpp"
#include "maze_engine/search/a_star.hpp"
#include "simple_directmedia_layer.hpp"
#include "timer.hpp"
#include "sound_table.hpp"
#include "maze_engine/maze_generation_iterator.hpp"

namespace App {
	class Performer;
	extern std::optional<Performer> performer;
}

class App::Performer {
	public:

		enum struct MazeType : std::uint_least8_t { hexagon = 1u, square };

		enum struct SearchType : std::uint_least8_t { depth = 1u, breadth, greedy };

		enum struct SoundType : std::uint_least8_t { none = 0u, piano, synthesizer };

		using Seed = unsigned int;

		static SoundTable piano;

		static SoundTable synthesizer;

		enum struct State : std::uint_least8_t {
			generating = 1u, searching, backtracking, complete
		};

	private /* member state; initialized by the constructor */:

		std::variant<MazeEngine::SquareMaze, MazeEngine::HexagonMaze> mazeVariant;
		MazeEngine::Vector2 mazeStart;
		MazeEngine::Vector2 mazeEnd;
		/*
			Careful, as the maze generation iterator holds a reference to the maze.
		*/
		MazeEngine::MazeGenerationIterator mazeGenerationIterator;
		/*
			Careful, as the maze search iterator holds a pointer to the maze.
		*/
		std::variant<
			MazeEngine::DepthFirstSearchIterator,
			MazeEngine::BreadthFirstSearchIterator,
			MazeEngine::GreedyBestFirstSearchIterator
		> mazeSearchIteratorVariant;
		Timer timer;
		SoundTable const *soundInstrument;
		/*
			`trailEdge->first` is the child vertex
			`trailEdge->second` is the parent vertex
		*/
		MazeEngine::Vector2::HashMap<MazeEngine::Vector2>::const_iterator trailEdge;

	private /* member state; initialized here */:

		MazeEngine::Vector2::HashSet markedTileSet;
		MazeEngine::MazeGenerationIterator::Wall::HashSet markedWallSet;
		MazeEngine::Vector2::HashSet pathTileSet;

		State state{State::generating};

	public:

		[[nodiscard]] explicit Performer(
			MazeType const mazeType, int const mazeSizeHint,
			Seed const seed, bool const mazeWrap,
			SearchType const searchType,
			SoundType const soundType,
			UnsignedMilliseconds const sleepTimeMilliseconds
		);

		Performer() = delete;
		Performer(Performer const &) = delete;
		Performer & operator=(Performer const &) = delete;
		Performer(Performer &&) = delete;
		Performer & operator=(Performer &&) = delete;

		[[nodiscard]] FORCE_INLINE constexpr State getState() const { return state; }

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Vector2 const & getMazeStart() const {
			return mazeStart;
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Vector2 const & getMazeEnd() const {
			return mazeEnd;
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Maze const & getMaze() const {
			return std::visit(
				[](auto and(maze)) -> MazeEngine::Maze const & {
					return maze;
				},
				mazeVariant
			);
		}

		[[nodiscard]] FORCE_INLINE
		decltype(markedTileSet) const & getMarkedTileSet() const {
			return markedTileSet;
		}

		[[nodiscard]] FORCE_INLINE
		decltype(markedWallSet) const & getMarkedWallSet() const {
			return markedWallSet;
		}

		[[nodiscard]] FORCE_INLINE
		decltype(pathTileSet) const & getPathTileSet() const {
			return pathTileSet;
		}

		[[nodiscard]] FORCE_INLINE
		decltype(mazeVariant) const & getUnderlyingMaze() const {
			return mazeVariant;
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::MazeSearchIterator const & getMazeSearchIterator() const {
			return std::visit(
				[](auto and(mazeSearchIterator)) -> MazeEngine::MazeSearchIterator const & {
					return mazeSearchIterator;
				},
				mazeSearchIteratorVariant
			);
		}

		void update();

		[[nodiscard]] FORCE_INLINE
		MazeEngine::MazeGenerationIterator const & getMazeGenerationIterator() const {
			return mazeGenerationIterator;
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::UnionFinder::View getUnionFinderView() {
			return mazeGenerationIterator.getUnionFinderView();
		}

	private:

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Maze & getMaze() {
			return const_cast<MazeEngine::Maze &>(std::as_const(*this).getMaze());
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::MazeSearchIterator & getMazeSearchIterator() {
			return const_cast<MazeEngine::MazeSearchIterator &>(std::as_const(*this).getMazeSearchIterator());
		}

		void playSound(MazeEngine::Vector2 const vertex);
};

#endif
