#ifndef Application_Performer_hpp
#define Application_Performer_hpp

#include <variant>
#include <type_traits>

#include "maze_engine/maze/square.hpp"
#include "maze_engine/maze/hexagon.hpp"
#include "maze_engine/search/breadth_first.hpp"
#include "maze_engine/search/depth_first.hpp"
#include "maze_engine/search/greedy_best_first.hpp"
#include "maze_engine/search/a_star.hpp"
#include "simple_directmedia_layer.hpp"
#include "timer.hpp"
#include "sound_table.hpp"
#include "color.hpp"
#include "maze_engine/maze_generation_iterator.hpp"
#include "celebration_song.hpp"

namespace App {
	class Performer;
	extern std::optional<Performer> performer;
}

class App::Performer {
	public:

		enum struct MazeType : std::uint_least8_t { hexagon = 1u, square };

		enum struct SearchType : std::uint_least8_t { depth = 1u, breadth, greedy, aStar };

		enum struct SoundType : std::uint_least8_t { none = 0u, piano, synthesizer };
		static_assert(SoundType{} == SoundType::none);

		using SeedInt = unsigned int;
		static_assert(std::is_integral_v<SeedInt>);

		using HueFloat = decltype(HslaColor({}).getHue());
		static_assert(std::is_floating_point_v<HueFloat>);
		static_assert(std::is_same_v<HueFloat, double>);
		static_assert(std::is_fundamental_v<HueFloat>);

		static SoundTable piano      ;
		static SoundTable synthesizer;

		[[nodiscard]] static constexpr
		SoundTable const * dispatchSoundInstrument(SoundType const soundType) {
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
		}

		enum struct State : std::uint_least8_t {
			generating = 1u, searching, backtracking, celebrating, complete,
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
			MazeEngine::GreedyBestFirstSearchIterator,
			MazeEngine::AStarSearchIterator
		> mazeSearchIteratorVariant;
		Timer timer;
		/*
			Careful, this can be `nullptr`.
		*/
		SoundTable const *soundInstrument;
		decltype(SoundTable::makeRandomSoundPicker({})) randomSoundPicker;
		HueFloat baseHueOffset{0.0};
		/*
			`trailEdge->first` is the child vertex
			`trailEdge->second` is the parent vertex
		*/
		MazeEngine::Vector2::HashMap<MazeEngine::Vector2>::const_iterator trailEdge;

	private /* member state; initialized here */:
		MazeEngine::Vector2::HashSet markedTileSet;
		MazeEngine::MazeGenerationIterator::Wall::HashSet markedWallSet;
		MazeEngine::Vector2::HashSet pathTileSet;
		CelebrationSong celebrationSong;
		State state{State::generating};

	public:

		constexpr void pause() { state = State::complete; }

		[[nodiscard]] explicit Performer(
			MazeType const mazeType, int const mazeSizeHint,
			SeedInt const seed, bool const mazeWrap,
			std::size_t const excessWallPruneCountdown,
			SearchType const searchType,
			SoundType const soundType,
			HueFloat const paramBaseHueOffset,
			UnsignedMilliseconds const sleepTimeMilliseconds,
			bool const showMazeGeneration
		);

		constexpr void setSoundInstrument(SoundType const soundType) {
			soundInstrument = dispatchSoundInstrument(soundType);
		}

		constexpr App::SoundTable const * getSoundInstrument() const {
			return soundInstrument;
		}

		constexpr void setBaseHueOffset(HueFloat const paramBaseHueOffset) {
			baseHueOffset = paramBaseHueOffset;
		}

		constexpr void setTimeUpdateInterval(UnsignedMilliseconds const interval) {
			timer.interval = interval;
		}

		Performer() = delete;
		Performer(Performer const &) = delete;
		Performer & operator=(Performer const &) = delete;
		Performer(Performer &&) = delete;
		Performer & operator=(Performer &&) = delete;

		[[nodiscard]] FORCE_INLINE constexpr State getState() const { return state; }

		[[nodiscard]] FORCE_INLINE constexpr
		HueFloat getBaseHueOffset() const { return baseHueOffset; }

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Vector2 getMazeStart() const {
			return mazeStart;
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Vector2 getMazeEnd() const {
			return mazeEnd;
		}

		[[nodiscard]] FORCE_INLINE
		MazeEngine::Maze const & getMaze() const {
			return std::visit(
				[](auto const &maze) -> MazeEngine::Maze const & {
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
				[](MazeEngine::MazeSearchIterator const &mazeSearchIterator) -> MazeEngine::MazeSearchIterator const & {
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

		void playSound(MazeEngine::Vector2 const vertex) const;

		template<typename MazeT>
		constexpr std::enable_if_t<
			std::is_same_v<MazeT, MazeEngine:: SquareMaze> or
			std::is_same_v<MazeT, MazeEngine::HexagonMaze>,
		void> playSound(MazeEngine::Maze::Direction const direction) const {
			assert(soundInstrument != nullptr);
			if (soundInstrument == nullptr) return;

			constexpr std::size_t offset{std::tuple_size_v<SoundTable::Data> / 2u};
			static_assert(offset == 6u);

			if constexpr (std::is_same_v<MazeT, MazeEngine::SquareMaze>) {
				switch (direction) {
					case MazeEngine::Maze::Direction::north: soundInstrument->play(3u + offset); break;
					case MazeEngine::Maze::Direction::east : soundInstrument->play(1u + offset); break;
					case MazeEngine::Maze::Direction::south: soundInstrument->play(0u + offset); break;
					case MazeEngine::Maze::Direction::west : soundInstrument->play(2u + offset); break;
					default: break;
				}
			} else if constexpr (std::is_same_v<MazeT, MazeEngine::HexagonMaze>) {
				switch (direction) {
					case MazeEngine::Maze::Direction::northeast: soundInstrument->play(4u + offset); break;
					case MazeEngine::Maze::Direction::     east: soundInstrument->play(2u + offset); break;
					case MazeEngine::Maze::Direction::southeast: soundInstrument->play(0u + offset); break;
					case MazeEngine::Maze::Direction::southwest: soundInstrument->play(1u + offset); break;
					case MazeEngine::Maze::Direction::     west: soundInstrument->play(3u + offset); break;
					case MazeEngine::Maze::Direction::northwest: soundInstrument->play(5u + offset); break;
					default: break;
				}
			}
		}
};

#endif
