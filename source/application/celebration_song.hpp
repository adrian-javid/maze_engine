#ifndef Application_CelebrationSong_hpp
#define Application_CelebrationSong_hpp

#include <tuple>
#include "sound_table.hpp"

namespace App { class CelebrationSong; }

class App::CelebrationSong final {
	std::size_t noteIndex{};

	public:
		static constexpr std::size_t noteCount{std::tuple_size_v<App::SoundTable::Data>};
		static_assert(noteCount == 12u);

		[[nodiscard]] constexpr explicit CelebrationSong(): noteIndex{0u} {}

		void advance(App::SoundTable const *const soundInstrument) {
			if (isDone()) return;
			if (soundInstrument != nullptr) soundInstrument->play(noteIndex);
			noteIndex += 1u;
		}

		[[nodiscard]] constexpr bool isDone() const {
			assert(noteIndex <= noteCount);
			return noteIndex >= noteCount;
		}
};

#endif
