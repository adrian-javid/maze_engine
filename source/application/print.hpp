#ifndef Application_Print_hpp
#define Application_Print_hpp

#include "macros.hpp"
#include <iostream>

namespace App {
	template <std::ostream *outputStream=&std::cout, typename... ParamsT>
	inline void print(ParamsT and(...objects)) {
		(*outputStream << ... << std::forward<ParamsT>(objects));
	}

	template <std::ostream *outputStream=&std::cout, typename... ParamsT>
	inline void println(ParamsT and(...objects)) {
		return print<outputStream>(std::forward<ParamsT>(objects)..., '\n');
	}

	/*
		Print message with trailing newline character to standard error,
		then exit with failure code.
		
		If no arguments are passed,
		then no trailing newline character is printed.
	*/
	template<typename... ParamsT>
	[[noreturn]] inline void errorExit(ParamsT and(...args)) {
		if constexpr (sizeof...(args) >= 1u) {
			(std::cerr << ... << std::forward<ParamsT>(args)) << '\n';
		}
		std::exit(EXIT_FAILURE);
	}
}

#endif
