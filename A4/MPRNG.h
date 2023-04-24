// Thread-safe Pseudo Random-Number Generator : generate repeatable sequence of values that appear random.
//
// Interface :
//   PRNG(s) - set starting seed 
//   seed() - read seed
//   seed(s) - reset seed
//   prng() - generate random value in range [0,UINT_MAX]
//   prng(u) - generate random value in range [0,u]
//   prng(l,u) - generate random value in range [l,u]
//
// Examples : generate random number between 5-21
//   prng() % 17 + 5	values 0-16 + 5 = 5-21
//   prng( 16 ) + 5
//   prng( 5, 21 )

#pragma once

_Monitor MPRNG {
	unsigned int PRNGcnt = 0;
	unsigned int seed;
	unsigned int seed_r;								// random state
  public:
	void set_seed( unsigned int seed_ ) { seed_r = seed = seed_; srand( seed ); } // set seed
	_Nomutex unsigned int get_seed() const { return seed; }	// get seed
	MPRNG( unsigned int seed = 362436069 ) { set_seed( seed ); } // set seed
	unsigned int operator()() { PRNGcnt += 1; return rand_r( &seed_r ); } // [0,UINT_MAX]
	unsigned int operator()( unsigned int u ) { return operator()() % (u + 1); } // [0,u]
	unsigned int operator()( unsigned int l, unsigned int u ) { return operator()( u - l ) + l; } // [l,u]
	unsigned int calls() { return PRNGcnt; }
}; // MPRNG

// Local Variables: //
// mode: C++ //
// End: //
