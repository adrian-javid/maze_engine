#ifndef MazeEngine_MazeGenerationIterator_hpp
#define MazeEngine_MazeGenerationIterator_hpp

#include "maze.hpp"
#include "union_finder.hpp"
#include "application/macros.hpp"

#include <algorithm>
#include <random>
#include <cassert>
#include "maze_engine/auxiliary.hpp"

namespace MazeEngine { class MazeGenerationIterator; }

class MazeEngine::MazeGenerationIterator final {
	public:
		struct [[nodiscard]] Wall {
			Vector2 tileKey;
			Maze::Direction type;
			struct [[nodiscard]] Hash {[[nodiscard]] std::size_t operator()(Wall const &wall) const noexcept {
				return Aux::combineHashValues(
					Vector2::Hash{}(wall.tileKey),
					std::hash<Aux::Enum::Int<Maze::Direction>>{}(Aux::Enum::asInt(wall.type))
				);
			}};

			/// @brief An unordered set of `Vector2`.
			using HashSet = std::unordered_set<Wall, Hash>;

			[[nodiscard]]
			constexpr bool operator==(Wall const &other) const {
				return this->tileKey == other.tileKey and this->type == other.type;
			}

			[[nodiscard]]
			constexpr bool operator!=(Wall const &other) const { return not(*this == other); }
		};

		enum struct Result : std::uint_least8_t { none = 0u, didUnion = 1u, };
		static_assert(Result{} == Result::none);

		[[nodiscard]] explicit MazeGenerationIterator(Maze &paramMaze, unsigned int const seed, bool const wrap=true);

		Result advance();

		template <typename WallAccessListenerT>
		inline void advanceUntilUnionOrDone(WallAccessListenerT &&(wallAccessListener)) {
			static_assert(std::is_invocable_v<decltype(wallAccessListener), Wall const>);

			assert(not isDone());
			if (isDone()) return;

			for (
				Result result(Result::none);
				result == Result::none and not isDone();
				result = advance()
			) {
				Wall const *wall{getWall()};
				assert(wall != nullptr);
				std::forward<WallAccessListenerT>(wallAccessListener)(*wall);
			}
		}

		inline void advanceUntilUnionOrDone() { advanceUntilUnionOrDone([](Wall const) constexpr -> void {}); }

		[[nodiscard]] FORCE_INLINE inline bool isDone() const { return wallIterator == wallList.cend(); }

		[[nodiscard]] inline Wall const * getWall() const {
			if (isDone())
				return nullptr;
			else
				return &*wallIterator;
		}

		[[nodiscard]] inline Vector2::HashMap<UnionFinder::Identifier> const & getTileKeyIdentities() const {
			return identity;
		}

		[[nodiscard]] inline UnionFinder::View getUnionFinderView() {
			return UnionFinder::View(cyclePrevention);
		}

	private:
		Vector2::HashMap<UnionFinder::Identifier> identity;
		std::vector<Wall> wallList;
		UnionFinder cyclePrevention;
		Maze &maze;
		decltype(MazeGenerationIterator::wallList.cbegin()) wallIterator;
};

#endif
