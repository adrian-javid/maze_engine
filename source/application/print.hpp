#ifndef Application_Print_hpp
#define Application_Print_hpp

#include "macros.hpp"
#include <iostream>

namespace App {
	template <typename... ParamsT>
	FORCE_INLINE inline void print(ParamsT &&...objects) {
		(std::cout << ... << std::forward<ParamsT>(objects));
	}

	template <typename... ParamsT>
	FORCE_INLINE inline void println(ParamsT &&...objects) {
		return print(std::forward<ParamsT>(objects)..., '\n');
	}
}

#endif
