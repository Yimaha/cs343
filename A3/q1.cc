#include <iostream>
#include <vector>
#include <memory>										// unique_ptr
using namespace std;

#define VECTOR2

int tasks = 1, times = 10000000;						// default values

_Task Worker {
	enum { size = 100 };
	void main() {
		for ( int t = 0; t < times; t += 1 ) {
#if defined( DARRAY )
			unique_ptr<volatile int []> arr( new volatile int[size] );
			for ( int i = 0; i < size; i += 1 ) arr[i] = i;
#elif defined( VECTOR1 )
			vector<int> arr( size );
			for ( int i = 0; i < size; i += 1 ) arr.at(i) = i;
#elif defined( VECTOR2 )
			vector<int> arr;
			for ( int i = 0; i < size; i += 1 ) arr.push_back(i);
#elif defined( STACK )
			volatile int arr[size] __attribute__ (( unused )); // prevent unused warning
			for ( int i = 0; i < size; i += 1 ) arr[i] = i;
#else
	#error unknown data structure
#endif
		} // for
	} // Worker::main
}; // Worker

int main( int argc, char * argv[] ) {
	bool nosummary = ! getenv( "NOSUMMARY" );			// print summary ?

	try {												// process command-line arguments
		switch ( argc ) {
		  case 3:
			times = stoi( argv[2] ); if ( times <= 0 ) throw 1;
		  case 2:
			tasks = stoi( argv[1] ); if ( tasks <= 0 ) throw 1;
		} // switch
	} catch( ... ) {
		cout << "Usage: " << argv[0] << " [ tasks (> 0) [ times (> 0) ] ]" << endl;
		exit( 1 );
	} // try
	uProcessor p[tasks - 1];							// add CPUs (start with one)
	{
		Worker workers[tasks];							// add threads
	}
	if ( nosummary ) { malloc_stats(); }
} // main
