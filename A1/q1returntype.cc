#include <iostream>
#include <cstdlib>										// access: rand, srand
#include <cstring>	
#include <variant>									// access: strcmp
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

// note, that we check alternative @ every return to ensure we are processing the right value
// also note, that if is sufficient, since all blocks under if directly return and end the function
variant<double, Ex1> rtn1( double i ) {
  if ( Rand() % eperiod == 0 ) { return Ex1{ ( short int )Rand() }; } // 'throw' exception		
	return i + Rand();
}

variant<double, Ex1, Ex2> rtn2( double i ) {
  if ( Rand() % eperiod == 0 ) { return Ex2{ Rand() }; } // 'throw' exception		
    variant<double, Ex1> result = rtn1( i );
  if (holds_alternative<double>(result)) { return get<double>(result) + Rand(); }  // case when no exception
    return get<Ex1>(result); // only possible outcome
}

variant<double, Ex1, Ex2, Ex3> rtn3( double i ) {
  if ( Rand() % eperiod == 0 ) { return Ex3{ Rand() }; } // 'throw' exception			
    variant<double, Ex1, Ex2> result = rtn2( i );
  if (holds_alternative<double>(result)) { return get<double>(result) + Rand(); }  // case when no exception
  if (holds_alternative<Ex1>(result)) { return get<Ex1>(result); }  // case when Ex1
    return get<Ex2>(result); // case when Ex2
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
        variant<double, Ex1, Ex2, Ex3> result = rtn3( i );
        // handle exception and regular result
        if (holds_alternative<Ex1>(result)) { ev1 += get<Ex1>(result).code; ec1 += 1; }	
		else if(holds_alternative<Ex2>(result))  { ev2 += get<Ex2>(result).code; ec2 += 1; }
		else if(holds_alternative<Ex3>(result)) { ev3 += get<Ex3>(result).code; ec3 += 1; }	
        else {rv += get<double>(result); rc+=1;}
	} // for
	cout << "randcnt " << randcnt << endl;
	cout << "normal result " << rv << " exception results " << ev1 << ' ' << ev2 << ' ' << ev3 << endl;
	cout << "calls "  << rc << " exceptions " << ec1 << ' ' << ec2 << ' ' << ec3 << endl;
}