#include <iostream>
#include <cstdlib>										// access: rand, srand
#include <cstring>										// access: strcmp
using namespace std;
#include <unistd.h>										// access: getpid

#ifdef NOOUTPUT
#define PRINT( stmt )
#else
#define PRINT( stmt ) stmt
#endif // NOOUTPUT

struct E {};											// exception type
long int eperiod = 100, excepts = 0, calls = 0, dtors = 0, depth = 0; // counters
// Comment dtor increment to match with longjmp, otherwise diff does not work.
PRINT( struct T { ~T() { /*dtors += 1;*/ } }; )

long int Ackermann( long int m, long int n, long int depth ) {
	calls += 1;
	if ( m == 0 ) {
		if ( rand() % eperiod <= 2 ) { PRINT( T t; ) excepts += 1; throw E(); } // replace
		return n + 1;
	} else if ( n == 0 ) {
		try { return Ackermann( m - 1, 1, depth + 1 );	// replace
		} catch( E ) {									// replace
			PRINT( cout << " depth " << depth << " E1 " << m << " " << n << " |" );
			if ( rand() % eperiod <= 1 ) { PRINT( T t; ) excepts += 1; throw E(); }
		} // try
		PRINT( cout << " E1X " << m << " " << n << endl );
	} else {
		try { return Ackermann( m - 1, Ackermann( m, n - 1, depth + 1 ), depth + 1 ); // replace
		} catch( E ) {									// replace
			PRINT( cout << " depth " << depth << " E2 " << m << " " << n << " |" );
			if ( rand() % eperiod == 0 ) { PRINT( T t; ) excepts += 1; throw E(); }
		} // try
		PRINT( cout << " E2X " << m << " " << n << endl );
	} // if
	return 0;											// recover by returning 0
}
int main( int argc, char * argv[] ) {
	long int m = 4, n = 6, seed = getpid();				// default values
	try {												// process command-line arguments
		switch ( argc ) {
		  case 5: if ( strcmp( argv[4], "d" ) != 0 ) {	// default ?
				eperiod = stoi( argv[4] ); if ( eperiod <= 0 ) throw 1; } // if
		  case 4: if ( strcmp( argv[3], "d" ) != 0 ) {	// default ?
				seed = stoi( argv[3] ); if ( seed <= 0 ) throw 1; } // if
		  case 3: if ( strcmp( argv[2], "d" ) != 0 ) {	// default ?
				n = stoi( argv[2] ); if ( n < 0 ) throw 1; } // if
		  case 2: if ( strcmp( argv[1], "d" ) != 0 ) {	// default ?
				m = stoi( argv[1] ); if ( m < 0 ) throw 1; } // if
		  case 1: break;								// use all defaults
		  default: throw 1;
		} // switch
	} catch( ... ) {
		cerr << "Usage: " << argv[0] << " [ m (>= 0) | d [ n (>= 0) | d"
			" [ seed (> 0) | d [ eperiod (> 0) | d ] ] ] ]" << endl;
		exit( EXIT_FAILURE );
	} // try
	srand( seed );										// seed random number
	try {												// replace
		PRINT( cout << "Arguments " << m << " " << n << " " << seed << " " << eperiod << endl );
		long int val = Ackermann( m, n, 0 );
		PRINT( cout << "Ackermann " << val << endl );
	} catch( E ) {										// replace
		PRINT( cout << "E3" << endl );
	} // try
	cout << "calls " << calls << " exceptions " << excepts << " destructors " << dtors << endl;
}
