#include <iostream>
using namespace std;

static volatile long int shared = 0;					// volatile to prevent dead-code removal
static long int iterations = 100000000;

_Task increment {
	void main() {
		for ( int i = 0; i < iterations; i += 1 ) {
			shared += 1;								// two increments to increase pipeline size
			shared += 1;
		} // for
	} // increment::main
}; // increment

int main( int argc, char * argv[] ) {
	int processors = 1;
	try {												// process command-line arguments
		switch ( argc ) {
		  case 3: processors = stoi( argv[2] ); if ( processors <= 0 ) throw 1;
		  case 2: iterations = stoi( argv[1] ); if ( iterations <= 0 ) throw 1;
		  case 1: break;								// use defaults
		  default: throw 1;
		} // switch
	} catch( ... ) {
		cout << "Usage: " << argv[0] << " [ iterations (> 0) [ processors (> 0) ] ]" << endl;
		exit( 1 );
	} // try
	uProcessor p[processors - 1];						// create additional kernel threads
	{
		increment t[2];
	} // wait for tasks to finish
	cout << "shared:" << shared << endl;
} // main

// Local Variables: //
// tab-width: 4 //
// End: //
