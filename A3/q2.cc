
#include "q2mergesort.h"
#include <cstdlib> // access: rand, srand
#include <cstring> // access: strcmp
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

typedef TYPE T; // define the general type

unsigned int uDefaultStackSize() {
    return 512 * 1000; // set all task stack-size to 512K
}

int main( int argc, char *argv[] ) {
    istream *infile = &cin;   // default value
    ostream *outfile = &cout; // default value
    int depth = 0;            // default value
    int size = -1;            // if size is -1 means no -t flag is passed
    try {
        if ( argc < 1 ) { // we are never suppose to get no arg c
            throw 1;
        } // if
        // if we just have 1 argument, we go default
        if ( argc > 1 ) {
            if ( strcmp( argv[1], "-t" ) == 0 ) {
                // we are going to read the rest assuming -t
                switch ( argc ) {
                case 4:
                    depth = stoi( argv[3] ); // default depth
                    if ( depth < 0 ) throw 1;
                case 3:
                    size = stoi( argv[2] ); // default size
                    if ( size < 0 ) throw 1;
                case 2:
                    break; // use all defaults
                default:
                    throw 1;
                }
            } else {
                // now we assume we are in sort mode
                switch ( argc ) {
                case 4:
                    depth = stoi( argv[3] );
                    if ( depth < 0 ) throw 1;
                case 3:
                    if ( strcmp( argv[2], "d" ) != 0 ) {
                        try {
                            outfile = new ofstream( argv[2] );
                        } catch ( uFile::Failure & ) { // open failed ?
                            cerr << "Error! Could not open output file \"" << argv[2] << "\"" << endl;
                            throw 1;
                        } // try
                    }
                case 2:
                    if ( strcmp( argv[1], "d" ) != 0 ) {
                        try {
                            infile = new ifstream( argv[1] );
                        } catch ( uFile::Failure & ) { // open failed ?
                            cerr << "Error! Could not open unsorted input file \"" << argv[1] << "\"" << endl;
                            throw 1;
                        } // try
                    }
                case 1:
                    break; // use all defaults
                default:
                    throw 1;
                }
            } // if
        }

    } catch ( ... ) {
        cerr << "Usage: " << argv[0] << " ( [ unsorted-file | 'd' [ sorted-file | 'd' [ depth (>= 0) ] ] ] | -t size (>= 0) [ depth (>= 0) ] ) " << endl;
        exit( EXIT_FAILURE );
    } // try

    if ( size == -1 ) {
        for ( ;; ) {
            int length;
            *infile >> length;
          if ( infile->eof() || infile->fail() ) break;
            T *input = new T[length];
            for ( int i = 0; i < length; i++ ) {
                *infile >> input[i];
                *outfile << input[i];
                if ( (i + 1) % 22 == 0 && i != 0 ) {
                    *outfile << endl << "  ";
                } else if (i != length - 1) {
                    *outfile << " ";
                }  // if
            } // for
            *outfile << endl;
            mergesort( input, length, depth );
            for ( int i = 0; i < length; i++ ) {
                *outfile << input[i];
                if ( (i + 1) % 22 == 0 && i != 0 ) {
                    *outfile << endl << "  ";
                } else if (i != length - 1) {
                    *outfile << " ";
                } // if
            } // for
            *outfile << endl << endl;
            delete[] input; // we are done with input, remove it
        } // for
    } else {
        int *input = new int[size];
        for ( int i = 0; i < size; i++ ) {
            input[i] = size - i;
        } // for
        unsigned int times = sqrt( size );
        for ( unsigned int counter = 0; counter < times; counter += 1 ) {
            swap( input[0], input[rand() % size] );
        } // for
        *outfile << endl;
        uProcessor p[( 1 << depth ) - 1] __attribute__( ( unused ) );
        uTime start = uClock::currTime();
        mergesort( input, size, depth );
        cout << "Sort time " << uClock::currTime() - start << " sec." << endl;
    } // if

    if ( infile != &cin ) delete infile;    // close file, do not delete cin!
    if ( outfile != &cout ) delete outfile; // close file, do not delete cout!
} // main