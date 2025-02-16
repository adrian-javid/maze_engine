#ifndef Application_AudioData_hpp
#define Application_AudioData_hpp

#include <array>

namespace App::AudioData {
	using SimpleSound = std::array<unsigned char, std::size_t{783406u}>;

	struct Piano {
		struct Low {
			static SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
		};
		struct High {
			static SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
		};
	};

	struct Synthesizer {
		struct Low {
			static SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
		};
		struct High {
			static SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
		};
	};
}

#endif

