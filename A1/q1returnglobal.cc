#include <iostream>
#include <cstdlib>										// access: rand, srand
#include <cstring>										// access: strcmp
#include <variant>
using namespace std;
#include <unistd.h>										// access: getpid

struct Ex1 { short int code; };
struct Ex2 { int code; };
struct Ex3 { long int code; };

typedef enum Tag { NORMAL = 0, EX1 = 1, EX2 = 2, EX3 = 3 } Tag;

long int global_status_flag; // keep track of the exception value
Tag which_exception = NORMAL; // keep track of which exception we got

int eperiod = 10000;									// exception period
int randcnt = 0;
int Rand() {
	randcnt += 1;
	return rand();
}

double rtn1( double i ) {
  if ( Rand() % eperiod == 0 ) {
    global_status_flag = (short int)Rand(); 
    which_exception = EX1; 
    return 0; // note on exception case we return 0 since it is directly ignored anyway
  } // if
	return i + Rand();
}

double rtn2( double i ) {
  if ( Rand() % eperiod == 0 ) {
    global_status_flag = Rand(); 
    which_exception = EX2; 
    return 0; // note on exception case we return 0 since it is directly ignored anyway
  }	
  	double result = rtn1( i );
  if (which_exception == NORMAL) { return result + Rand(); } // note that we check if any exception been raised or not
	return 0; // exception found, return 0
}

double rtn3( double i ) {
  if ( Rand() % eperiod == 0 ) {
    global_status_flag = Rand(); 
    which_exception = EX3; 
    return 0; // note on exception case we return 0 since it is directly ignored anyway
  }	// if
    double result = rtn2( i );
  if (which_exception == NORMAL) { return result + Rand(); } // note that we check if any exception been raised or not
	return 0; // exception found, return 0
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

	double rv = 0.0;
	int ev1 = 0, ev2 = 0, ev3 = 0;
	int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

	for ( int i = 0; i < times; i += 1 ) {
		which_exception = NORMAL;
		double result = rtn3( i ); 
        if (which_exception == EX1) { ev1 += global_status_flag; ec1 += 1; }
        else if(which_exception == EX2) { ev2 += global_status_flag; ec2 += 1; } 
        else if(which_exception == EX3) { ev3 += global_status_flag; ec3 += 1; } 	
        else { rv += result; rc += 1; }
	} // for
	cout << "randcnt " << randcnt << endl;
	cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
	cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}