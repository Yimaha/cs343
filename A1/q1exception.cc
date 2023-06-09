#include <iostream>
#include <cstdlib>										// access: rand, srand
#include <cstring>										// access: strcmp
using namespace std;
#include <unistd.h>										// access: getpid

struct Ex1 { short int code; };
struct Ex2 { int code; };
struct Ex3 { long int code; };

int eperiod = 10000;									// exception period

int randcnt = 0;
int Rand() {
	randcnt += 1;
	return rand();
}
double rtn1( double i ) {
  if ( Rand() % eperiod == 0 ) throw Ex1{ (short int)Rand() }; // replace
	return i + Rand();
}
double rtn2( double i ) {
  if ( Rand() % eperiod == 0 ) throw Ex2{ Rand() };		// replace
	return rtn1( i ) + Rand();
}
double rtn3( double i ) {
  if ( Rand() % eperiod == 0 ) throw Ex3{ Rand() };		// replace
	return rtn2( i ) + Rand();
}
int main( int argc, char * argv[] ) {
	int times = 100000000, seed = getpid();				// default values
	try {
		switch ( argc ) {
		  case 4: if ( strcmp( argv[3], "d" ) != 0 ) {	// default ?
				seed = stoi( argv[3] ); if ( seed <= 0 ) throw 1;
			} // if
		  case 3: if ( strcmp( argv[2], "d" ) != 0 ) {	// default ?
				eperiod = stoi( argv[2] ); if ( eperiod <= 0 ) throw 1;
			} // if
		  case 2: if ( strcmp( argv[1], "d" ) != 0 ) {	// default ?
				times = stoi( argv[1] ); if ( times <= 0 ) throw 1;
			} // if
		  case 1: break;								// use all defaults
		  default: throw 1;
		} // switch
	} catch( ... ) {
		cerr << "Usage: " << argv[0] << " [ times > 0 | d [ eperiod > 0 | d [ seed > 0 | d ] ] ]" << endl;
		exit( EXIT_FAILURE );
	} // try
	srand( seed );
	//cout << times << ' ' << eperiod << ' ' << seed << endl;

	double rv = 0.0;
	int ev1 = 0, ev2 = 0, ev3 = 0;
	int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

	for ( int i = 0; i < times; i += 1 ) {
		try { rv += rtn3( i ); rc += 1; }				// replace
		// analyse exception
		catch( Ex1 ev ) { ev1 += ev.code; ec1 += 1; }	// replace
		catch( Ex2 ev ) { ev2 += ev.code; ec2 += 1; }	// replace
		catch( Ex3 ev ) { ev3 += ev.code; ec3 += 1; }	// replace
	} // for
	cout << "randcnt " << randcnt << endl;
	cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
	cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}