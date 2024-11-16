#include "sound_table.hpp"

#include "macros.hpp"
#include <cassert>
#include <array>

void ::App::SoundTable::freeAllChunks() {
	for (auto iterator(table.begin()); iterator != table.end(); ++iterator) {
		if (Mix_Chunk *const chunk{*iterator}; chunk != nullptr) {
			Mix_FreeChunk(chunk);
			*iterator = nullptr;
		}
	}
}

void ::App::SoundTable::put(std::size_t const identifier, AudioDataView const view) {
	assert(identifier < std::tuple_size_v<Data>);
	assert(view.getSize() <= std::numeric_limits<int>::max());

	// Read & write structure.
	SDL_RWops *audioSource{SDL_RWFromConstMem(view.getData(), static_cast<int>(view.getSize()))};

	SUPPRESS_WARNINGS({
		SDL_assert(audioSource != nullptr);
	})

	if (table[identifier] != nullptr) {
		Mix_FreeChunk(table[identifier]);
	}

	table[identifier] = Mix_LoadWAV_RW(audioSource, /* free audio source */SDL_TRUE);
	assert(table[identifier] != nullptr);
}

void ::App::SoundTable::play(std::size_t const identifier) const {
	assert(identifier < std::tuple_size_v<Data>);

	SUPPRESS_WARNINGS({
		SDL_assert(table[identifier] != nullptr);
	})

	int const channel{
		Mix_PlayChannel(
			/* channel */ static_cast<int>(identifier),
			/* chunk   */ table[identifier],
			/* loops   */ 0
		)
	};

	static constexpr int soundCouldNotBePlayed{-1};

	SUPPRESS_WARNINGS({
		SDL_assert(channel != soundCouldNotBePlayed);
	})
}
