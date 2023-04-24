#include <stdlib.h>										// access: rand, srand
#include <stdio.h>										// access: strcmp
#include <unistd.h>										// access: getpid
#include <string.h>

union Data {
    double result;
    long int error;
};

typedef enum Tag { NORMAL = 0, EX1 = 1, EX2 = 2, EX3 = 3 } Tag;

typedef struct TaggedUnion {
    union Data data; // data recorder
	Tag tag; // 0 means normal, 1 means ex1, 2 means ex2, 3 means ex3
} TaggedUnion;

int eperiod = 10000;									// exception period
int randcnt = 0;

int Rand() {
		randcnt += 1;
		return rand();
}

TaggedUnion rtn1( double i ) {
    union Data data;
  if ( Rand() % eperiod == 0 ) {
	data.error = (short int)Rand();
	TaggedUnion val = { data, EX1 }; // return EX1
	return val;
  }
    data.result = i + Rand();
	TaggedUnion val = { data, NORMAL }; // return normal
	return val;
}

TaggedUnion rtn2( double i ) {
    union Data data;
  if ( Rand() % eperiod == 0 ) {
	data.error = Rand() ;
	TaggedUnion val = { data, EX2 }; // return EX2
	return val;
  }
    TaggedUnion result = rtn1( i );
  if (result.tag == 0) {
	result.data.result += Rand();   // if normal append rand result
  };                                // otherwise just propage exception back
	return result; 
}

TaggedUnion rtn3( double i ) {
	union Data data;
  if ( Rand() % eperiod == 0 ) {
	data.error = Rand();
	TaggedUnion val = { data, EX3 }; // return EX3
	return val;
  }		// replace
    TaggedUnion result = rtn2( i );
  if (result.tag == 0) {
	result.data.result += Rand();   // if normal append rand result
  };                                // otherwise just propage exception back
	return result;
}

int main( int argc, char * argv[] ) {
	int times = 100000000, seed = getpid();				// default values
	switch ( argc ) {
		case 4: if ( strcmp( argv[3], "d" ) != 0 ) {	// default ?
				    seed = strtoll( argv[3], NULL, 10); 
                    if ( seed <= 0 ) { goto Exit; }
			    } // if
		case 3: if ( strcmp( argv[2], "d" ) != 0 ) {	// default ?
				    eperiod = strtoll( argv[2], NULL, 10); 
                    if ( eperiod <= 0 ) { goto Exit; }
			    } // if
		case 2: if ( strcmp( argv[1], "d" ) != 0 ) {	// default ?
				    times = strtoll( argv[1], NULL, 10); 
                    if ( times <= 0 ) { goto Exit; }
			    } // if
		case 1: break;								// use all defaults
		default: goto Exit;
	} //switch
	goto No_Exit; // since no error found, continue
  Exit: // Exception flag
	fprintf(stderr, "Usage: %s  [ times > 0 | d [ eperiod > 0 | d [ seed > 0 | d ] ] ]\n", argv[0]);
	exit( EXIT_FAILURE );
  No_Exit: // Normal Flag
	srand( seed );

	double rv = 0.0;
	int ev1 = 0, ev2 = 0, ev3 = 0;
	int rc = 0, ec1 = 0, ec2 = 0, ec3 = 0;

	for ( int i = 0; i < times; i += 1 ) {
      	TaggedUnion result = rtn3( i );
        // handle exception and regular result
        if(result.tag == EX1) { ev1 += result.data.error; ec1 += 1; }	
        else if(result.tag == EX2) { ev2 += result.data.error; ec2 += 1; }	
        else if(result.tag == EX3) { ev3 += result.data.error; ec3 += 1; }	
        else {rv += result.data.result; rc += 1;}
	} 
	printf("randcnt %d\n", randcnt);
	printf("normal result %g exception results %d %d %d\n", rv, ev1, ev2, ev3);
	printf("calls %d exceptions %d %d %d\n", rc, ec1, ec2, ec3);
}