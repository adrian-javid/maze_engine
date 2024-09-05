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

namespace App {
	class Performer;
	extern std::optional<Performer> performer;
}

class App::Performer {
	public:

		enum struct MazeType : std::uint_least8_t { hexagon = 1u, square };

		enum struct SearchType : std::uint_least8_t { depth = 1u, breadth, greedy };

		using Seed = unsigned int;

		static SoundTable piano;

		static SoundTable synthesizer;

	private:

		std::variant<MazeEngine::SquareMaze, MazeEngine::HexagonMaze> mazeVariant;
		MazeEngine::Vector2 mazeStart;
		MazeEngine::Vector2 mazeEnd;
		/*
			Careful, as the maze search iterator holds a pointer to the maze.
		*/
		std::variant<
			MazeEngine::DepthFirstSearchIterator,
			MazeEngine::BreadthFirstSearchIterator,
			MazeEngine::GreedyBestFirstSearchIterator
		> mazeSearchIteratorVariant;
		Timer timer;

		MazeEngine::Vector2::HashSet markedTileSet;
		MazeEngine::Vector2::HashSet pathTileSet;
		/*
			`edge->first` is the child vertex
			`edge->second` is the parent vertex
		*/
		MazeEngine::Vector2::HashMap<MazeEngine::Vector2>::const_iterator edge{};
		std::int_fast8_t soundIndex{0u};
		enum struct State : std::uint_least8_t {
			searching = 1u, backtracking, complete
		} state{State::searching};
		SoundTable const *soundInstrument{&piano};

	public:

		[[nodiscard]] explicit Performer(
			MazeType const mazeType, int const mazeSizeHint,
			Seed const seed, bool const mazeWrap,
			SearchType const searchType,
			UnsignedMilliseconds const sleepTimeMilliseconds
		);

		Performer() = delete;
		Performer(Performer const &) = delete;
		Performer & operator=(Performer const &) = delete;
		Performer(Performer &&) = delete;
		Performer & operator=(Performer &&) = delete;

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

	private:

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Maze & getMaze() {
			return const_cast<MazeEngine::Maze &>(std::as_const(*this).getMaze());
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::MazeSearchIterator & getMazeSearchIterator() {
			return const_cast<MazeEngine::MazeSearchIterator &>(std::as_const(*this).getMazeSearchIterator());
		}

		void playSound(MazeEngine::Vector2 const &vertex);
};

#endif
