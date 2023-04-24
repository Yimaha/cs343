#include "q1Filter.h"
#include "q1Reader.h"
#include "q1Writer.h"
#include <fstream>
#include <iostream>
#include <string> // strtol
#include <vector>
using namespace std; // direct access to std

int main( int argc, char *argv[] ) {
    // MUST BE INT (NOT UNSIGNED) TO CORRECTLY TEST FOR NEGATIVE VALUES
    istream *infile = &cin;   // default value
    ostream *outfile = &cout; // default value

    vector<string> inputs;
    try {
        for ( int i = 1; i < argc; i++ ) {
            if ( strcmp( argv[i], "-c" ) == 0 || strcmp( argv[i], "-e" ) == 0 ) {
                inputs.push_back( argv[i + 1] );
                inputs.push_back( argv[i] );
                i += 1;
            } else if ( strcmp( argv[i], "-s" ) == 0 ) {
                inputs.push_back( argv[i] );
            } else if ( infile == &cin ) {
                if ( argc - i > 2 ) { // there should not be more than 1 parameter after this
                    throw 1;
                }
                try { // open input file first as output creates file
                    infile = new ifstream( argv[i] );
                } catch ( uFile::Failure & ) { // open failed ?
                    cerr << "Error! Could not open input file \"" << argv[i] << "\"" << endl;
                    throw 1;
                } // try
            } else if ( outfile == &cout ) {
                if ( argc - i > 1 ) { // there should not be any more parameter after this
                    throw 1;
                }
                try {
                    outfile = new ofstream( argv[i] );
                } catch ( uFile::Failure & ) { // open failed ?
                    cerr << "Error! Could not open output file \"" << argv[i] << "\"" << endl;
                    throw 1;
                } // try
            } else {
                throw 1;
            } // if
        } // for
    } catch ( ... ) {
        cerr << "Usage: " << argv[0] << " [ -filter-options ... ] [ infile [outfile] ]" << endl;
        exit( EXIT_FAILURE ); // TERMINATE
    } // try

    *infile >> noskipws; // turn off white space skipping during input

    vector<Filter *> filters; // due to inheritance, we have to use pointer.
    filters.push_back( new Writer( *outfile ) );
    for ( int i = inputs.size() - 1; i >= 0; i-- ) {
        if ( inputs[i] == "-c" ) {
            bool upper = inputs[i - 1] == "u";
            filters.insert( filters.begin(), new FilterCase( filters[0], upper ) );
            i--;
        } else if ( inputs[i] == "-e" ) {
            int number = stoi( inputs[i - 1] );
            filters.insert( filters.begin(), new FilterNumber( filters[0], number ) );
            i--;
        } else if ( inputs[i] == "-s" ) {
            filters.insert( filters.begin(), new FilterCapitalize( filters[0] ) );
        } // if
    } // for
    Reader reader = Reader( filters[0], *infile ); // this should automatically start the parsing

    for ( Filter *filter : filters ) { // once we done with all the filters, deletes them 1 by 1
        delete filter; // note that filter themselves don't need defined destructor (cause they don't need to)
    } // for

    if ( infile != &cin ) delete infile;    // close file, do not delete cin!
    if ( outfile != &cout ) delete outfile; // close file, do not delete cout!
} // main
