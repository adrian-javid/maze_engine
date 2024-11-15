#ifndef Application_AudioData_hpp
#define Application_AudioData_hpp

#include <array>

namespace App::AudioData {
	using SimpleSound = std::array<unsigned char, std::size_t{783406u}>;

	namespace Piano::Low {
		extern SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
	}

	namespace Synthesizer::Low {
		extern SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
	}

	namespace Piano::High {
		extern SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
	}

	namespace Synthesizer::High {
		extern SimpleSound const first, third, fifth, highFirst, highThird, highFifth;
	}
}

#endif

