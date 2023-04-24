#include "MPRNG.h"
#include "q2tallyVotes.h"
#include <iostream>
using namespace std;

MPRNG mprng;

int main( int argc, char *argv[] ) {
    int voters = 6, group = 3, votes = 1, seed = 1, processors = 1;
    try {
        switch ( argc ) {
        case 6:
            if ( strcmp( argv[5], "d" ) != 0 ) { // default processors
                processors = stoi( argv[5] );
                if ( processors <= 0 ) throw 1;
            } // if
        case 5:
            if ( strcmp( argv[4], "d" ) != 0 ) { // default processor
                seed = stoi( argv[4] );
                if ( seed <= 0 ) throw 1;
            } // if
        case 4:
            if ( strcmp( argv[3], "d" ) != 0 ) { // default votes
                votes = stoi( argv[3] );
                if ( votes <= 0 ) throw 1;
            } // if
        case 3:
            if ( strcmp( argv[2], "d" ) != 0 ) { // default group
                group = stoi( argv[2] );
                if ( group <= 0 ) throw 1;
            } // if
        case 2:
            if ( strcmp( argv[1], "d" ) != 0 ) { // default voters
                voters = stoi( argv[1] );
                if ( voters <= 0 ) throw 1;
            } // if
        case 1:
            break; // use all defaults
        default:
            throw 1;
        } // switch
    } catch ( ... ) {
        cerr << "Usage: " << argv[0]
             << " [ voters (> 0) | 'd' (default 6) [ group (> 0) | 'd' (default 3) [ votes (> 0) | 'd' (default 1) [ "
                "seed (> 0), | 'd' (default random) [ processors (> 0), | 'd' (default 1) ] ] ] ] ]"
             << endl;
        exit( EXIT_FAILURE );
    } // try
    uProcessor p[processors - 1];
    mprng.set_seed( seed );
    Printer printer( voters );
    TallyVotes voteTallier = TallyVotes( voters, group, printer );
    Voter *voter[voters];
    for ( int i = 0; i < voters; i++ ) {
        voter[i] = new Voter( i, votes, voteTallier, printer );
    }

    for ( int i = 0; i < voters; i++ ) {
        delete voter[i];
    }
}