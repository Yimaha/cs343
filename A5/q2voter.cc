// this files constains all functions that is shared amount 3 implementations

#include "q2tallyVotes.h"
#include <math.h>
using namespace std;

#ifdef NOOUTPUT
#define PRINT( stmt )
#else
#define PRINT( stmt ) stmt
#endif // NOOUTPUT

Voter::Voter( unsigned int id, unsigned int nvotes, TallyVotes &voteTallier, Printer &printer )
    : id( id ), nvotes( nvotes ), voteTallier( voteTallier ), printer( printer ) {}

void Voter::main() {
    yield( mprng( 19 ) ); // yeild 0 - 19
    try {
        for ( unsigned int i = 0; i < nvotes; i++ ) {
            printer.print( id, Voter::States::Start );                // S
            yield( mprng( 4 ) );                                      // yield 0 - 4
            TallyVotes::Tour result = voteTallier.vote( id, cast() ); // toss in your vote
            yield( mprng( 4 ) );                                      // yield 0 - 4
            printer.print( id, Voter::States::Going, result );        // G
        }

    } catch ( TallyVotes::Failed & ) {
        printer.print( id, Voter::States::Failed ); // X
    }
    voteTallier.done(
#if defined( TASK ) // task solution
        id
#endif
    );
    printer.print( id, Voter::States::Terminated ); // F
}