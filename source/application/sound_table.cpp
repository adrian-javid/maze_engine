#include "sound_table.hpp"

#include "macros.hpp"
#include <cassert>
#include <array>

namespace App::SoundTable {
	static std::array<Mix_Chunk *, size> table{};
}

void ::App::SoundTable::freeAllChunks() {
	for (auto iterator(table.begin()); iterator != table.end(); ++iterator) {
		if (Mix_Chunk *const chunk{*iterator}; chunk != nullptr) {
			Mix_FreeChunk(chunk);
			*iterator = nullptr;
		}
	}
}

void ::App::SoundTable::put(std::size_t const identifier, DataView const dataView) {
	auto const [data, dataSize]{dataView};
	
	assert(identifier < size);
	assert(dataSize <= std::numeric_limits<int>::max());

	// Read & write structure.
	SDL_RWops *audioSource{SDL_RWFromConstMem(data, static_cast<int>(dataSize))};

	SUPPRESS_WARNINGS({
		SDL_assert(audioSource != nullptr);
	})

	if (table[identifier] != nullptr) {
		Mix_FreeChunk(table[identifier]);
	}

	table[identifier] = Mix_LoadWAV_RW(audioSource, SDL_TRUE);
}

void ::App::SoundTable::play(std::size_t const identifier) {
	assert(identifier < size);

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
