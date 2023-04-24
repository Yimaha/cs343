#ifndef __POTATO__
#define __POTATO__ 1

#include "PRNG.h"

class Potato {
	// YOU ADD MEMBERS HERE
    PRNG* prng;
    unsigned int currentTick;
  public:
	_Event Explode {};
	Potato( PRNG & prng, unsigned int maxTicks = 10 );
	void reset( unsigned int maxTicks = 10 );
	void countdown();
};

#endif