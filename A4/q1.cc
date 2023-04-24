#include <iostream>
#include <string>
#include <sstream>
using namespace std;
#include <malloc.h>

int main( int argc, char *argv[] ) {
	int times = 5, size = 5;							// defaults
	bool nosummary = getenv( "NOSUMMARY" );				// print summary ?

	try {
		switch ( argc ) {
		  case 3: size = stoi( argv[2] ); if ( size <= 0 ) throw 1;
		  case 2: times = stoi( argv[1] ); if ( times <= 0 ) throw 1;
		  case 1: break;								// use defaults
		  default: throw 1;
		} // switch
	} catch( ... ) {
		cout << "Usage: " << argv[0] << " [ times (> 0) [ size (> 0) ] ]" << endl;
		exit( 1 );
	} // try

	enum : long int { C = 1234567890 };					// print multiple characters

	#if defined( ARRAY )
	struct S { long int i, j, k, l; };
	S buf[size];										// internal-data buffer
	#elif defined( STRING )
	string strbuf;										// external-data buffer
	#elif defined( STRSTREAM )
	stringstream ssbuf;									// external-data buffer
	#else
		#error unknown buffering style
	#endif

	for ( int i = 0; i < times; i += 1 ) {
		#if defined( ARRAY )
		for ( int i = 0; i < size; i += 1 ) buf[i] = (S){ C - i, C + i, C | i, C ^ i };
		for ( int i = 0; i < size; i += 1 ) cout << buf[i].i << '\t' << buf[i].j << '\t'
												 << buf[i].k << '\t' << buf[i].l << '\t';
		cout << endl;
		#elif defined( STRING )
		for ( int i = 0; i < size; i += 1 ) strbuf += to_string(C - i) + '\t' + to_string(C + i) + '\t'
												+ to_string(C | i) + '\t' + to_string(C ^ i) + '\t';
		cout << strbuf << endl;
		strbuf.clear();									// reset string
		#elif defined( STRSTREAM )
		for ( int i = 0; i < size; i += 1 ) ssbuf << (C - i) << '\t' << (C + i) << '\t'
												  << (C | i) << '\t' << (C ^ i) << '\t';
		cout << ssbuf.str() << endl;
		ssbuf.seekp( 0 );								// reset stream
		#else
			#error unknown buffering style
		#endif
	} // for

	if ( ! nosummary ) { malloc_stats(); }
}
