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

	private:

		std::variant<MazeEngine::SquareMaze, MazeEngine::HexagonMaze> mazeVariant;
		MazeEngine::Vector2 mazeStart;
		MazeEngine::Vector2 mazeEnd;
		std::variant<
			MazeEngine::DepthFirstSearchIterator,
			MazeEngine::BreadthFirstSearchIterator,
			MazeEngine::GreedyBestFirstSearchIterator
		> mazeSearchIteratorVariant;
		Uint32 sleepTime;

		MazeEngine::Vector2::HashSet markedTileSet;
		MazeEngine::Vector2::HashSet pathTileSet;
		/*
			`edge->first` is the child vertex
			`edge->second` is the parent vertex
		*/
		MazeEngine::Vector2::HashMap<MazeEngine::Vector2>::const_iterator edge;
		enum struct State : std::uint_least8_t {
			searching = 1u, backtracking, complete
		} state{State::searching};

		static std::optional<Performer> performer;

	public:

		[[nodiscard]] explicit
		Performer(
			MazeType const mazeType, int const mazeSize,
			unsigned int const seed, bool const mazeWrap,
			SearchType const searchType,
			decltype(Performer::sleepTime) sleepTimeMilliseconds
		);

		FORCE_INLINE static
		void initialize(Performer &&performerValue) {
			performer = std::move(performerValue);
		}

		/*
			Make sure to initialize the global performer before calling
			this function because otherwise will invoke undefined behavior
			when accessing the empty `std::optional`.
		*/
		[[nodiscard]] FORCE_INLINE static
		App::Performer const & get() {
			return *performer;
		}

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
				[](auto &&maze) -> MazeEngine::Maze const & {
					return maze;
				},
				mazeVariant
			);
		}

		[[nodiscard]] FORCE_INLINE
		decltype(markedTileSet) const &getMarkedTileSet() const {
			return markedTileSet;
		}

		decltype(pathTileSet) const &getPathTileSet() const {
			return pathTileSet;
		}

		[[nodiscard]] FORCE_INLINE
		decltype(mazeVariant) const & getUnderlyingMaze() const {
			return mazeVariant;
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::MazeSearchIterator const & getMazeSearchIterator() const {
			return std::visit(
				[](auto &&mazeSearchIterator) -> MazeEngine::MazeSearchIterator const & {
					return mazeSearchIterator;
				},
				mazeSearchIteratorVariant
			);
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::MazeSearchIterator & getMazeSearchIterator() {
			return const_cast<MazeEngine::MazeSearchIterator &>(std::as_const(*this).getMazeSearchIterator());
		}

		void update();

	private:

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Maze & getMaze() {
			return const_cast<MazeEngine::Maze &>(std::as_const(*this).getMaze());
		}
};

#endif
