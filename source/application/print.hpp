#ifndef Application_Print_hpp
#define Application_Print_hpp

#include "macros.hpp"
#include <iostream>

namespace App {
	template <typename... ParamsT>
	inline void print(ParamsT &&...objects) {
		(std::cout << ... << std::forward<ParamsT>(objects));
	}

	template <typename... ParamsT>
	inline void println(ParamsT &&...objects) {
		return print(std::forward<ParamsT>(objects)..., '\n');
	}

	/*
		Print message with trailing newline character to standard error,
		then exit with failure code.
		
		If no arguments are passed,
		then no trailing newline character is printed.
	*/
	template<typename... ParamsT>
	[[noreturn]] inline void errorExit(ParamsT &&...args) {
		if constexpr (sizeof...(args) >= 1u) {
			(std::cerr << ... << std::forward<ParamsT>(args)) << '\n';
		}
		std::exit(EXIT_FAILURE);
	}
}

#endif
