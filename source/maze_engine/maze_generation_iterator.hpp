#ifndef MazeEngine_MazeGenerationIterator_hpp
#define MazeEngine_MazeGenerationIterator_hpp

#include "maze.hpp"
#include "union_finder.hpp"
#include "application/macros.hpp"

#include <algorithm>
#include <random>
#include <cassert>

namespace MazeEngine { class MazeGenerationIterator; }

class MazeEngine::MazeGenerationIterator final {
	private:
		struct Wall { Vector2 tileKey; Maze::Direction type; };

	public:
		[[nodiscard]] explicit MazeGenerationIterator(Maze &paramMaze, unsigned int const seed, bool const wrap=true);

		void advance();

		[[nodiscard]] FORCE_INLINE inline bool isDone() { return wallIterator == wallList.cend(); }

	private:
		Vector2::HashMap<UnionFinder::Identifier> identity;
		std::vector<Wall> wallList;
		UnionFinder cyclePrevention;
		Maze &maze;
		decltype(MazeGenerationIterator::wallList.cbegin()) wallIterator;
};

#endif
