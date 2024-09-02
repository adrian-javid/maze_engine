#ifndef Application_AudioData_Synthesizer_MusicNotes_hpp
#define Application_AudioData_Synthesizer_MusicNotes_hpp

#include <array>

namespace App::AudioData::Synthesizer {
	using MusicNoteData = std::array<unsigned char, std::size_t{783406u}>;
	extern MusicNoteData a, b, c, d, e, f, g;
}

#endif

