#include <iostream>
#include <cstdlib>										// access: rand, srand
#include <cstring>										// access: strcmp
using namespace std;
#include <unistd.h>										// access: getpid
#include <csetjmp>

#ifdef NOOUTPUT
#define PRINT( stmt )
#else
#define PRINT( stmt ) stmt
#endif // NOOUTPUT

jmp_buf global_jump_tracker;                            // global variable used to track current jump location
struct E {};											// exception type
long int eperiod = 100, excepts = 0, calls = 0, dtors = 0, depth = 0; // counters
// Comment dtor increment to match with longjmp, otherwise diff does not work.
PRINT( struct T { ~T() { /* dtors += 1; */ } }; )

long int Ackermann( long int m, long int n, long int depth ) {
    // whenever we enter this function, it 'remembers' the location of 'catch'
    // by storing the corresponding jump location as a local variable
	jmp_buf prev_jump_tracker;
	memcpy( prev_jump_tracker, global_jump_tracker, sizeof( global_jump_tracker ) );
	calls += 1;
	if ( m == 0 ) {
		if ( rand() % eperiod <= 2 ) { PRINT( T t; ) excepts += 1; longjmp( global_jump_tracker, 1 ); } // special case where global == local
		return n + 1;
	} else if ( n == 0 ) {
		int jump = setjmp( global_jump_tracker ); // set the jump ('catch') before calling Ackermann
		if (jump != 0) { // handle exception
			PRINT( cout << " depth " << depth << " E1 " << m << " " << n << " |" );
			if ( rand() % eperiod <= 1 ) { PRINT( T t; ) excepts += 1; longjmp( prev_jump_tracker, 1 ); }
		} else {
			long int result = Ackermann( m - 1, 1, depth + 1);
			// reset the global_jump_tracker back to routine caller
			memcpy(global_jump_tracker, prev_jump_tracker, sizeof(prev_jump_tracker));
			return result;
		}
		PRINT( cout << " E1X " << m << " " << n << endl );
	} else {
		int jump = setjmp(global_jump_tracker); // set the jump ('catch') before calling Ackermann
		if (jump != 0) { // handle exception
			PRINT( cout << " depth " << depth << " E2 " << m << " " << n << " |" );
			if ( rand() % eperiod == 0 ) { PRINT( T t; ) excepts += 1; longjmp( prev_jump_tracker, 1 ); }
		} else {
			long int result = Ackermann( m - 1, Ackermann( m, n - 1, depth + 1 ), depth + 1 ); 
			// reset the global_jump_tracker back to routine caller
			memcpy(global_jump_tracker, prev_jump_tracker, sizeof(prev_jump_tracker));
			return result;
		}
		PRINT( cout << " E2X " << m << " " << n << endl );
	}
    // reset the global_jump_tracker back to routine caller
	memcpy(global_jump_tracker, prev_jump_tracker, sizeof(prev_jump_tracker));
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
    
    int jump = setjmp(global_jump_tracker); // set the jump ('catch') before calling Ackermann
  if(jump != 0) { // handle exception
    PRINT( cout << "E3" << endl );
  } else {
    PRINT( cout << "Arguments " << m << " " << n << " " << seed << " " << eperiod << endl );
    long int val = Ackermann( m, n, 0 );
    PRINT( cout << "Ackermann " << val << endl );
  } // if
	cout << "calls " << calls << " exceptions " << excepts << " destructors " << dtors << endl;
}
