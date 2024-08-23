#ifndef Application_App_hpp
#define Application_App_hpp

#include "simple_directmedia_layer.hpp"

// Application stuff.
namespace App {

	// Change in time.
	extern Uint64 deltaTime;

	/*
		Not thread safe.
	*/
	void mainLoop();
}


#endif
