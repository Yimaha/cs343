

#include "q2Potato.h"
#include "PRNG.h"
#include <iostream>
using namespace std;

Potato::Potato( PRNG &prng, unsigned int maxTicks ) : prng( &prng ) { reset( maxTicks ); }

void Potato::reset( unsigned int maxTicks ) {
    // decide a tick between max and 1
    currentTick = prng->operator()( 1, maxTicks );
    cout << "  POTATO goes off after " << currentTick << " tick";
    // making sure that s is appended if selected is not 1
    if ( currentTick != 1 ) {
        cout << "s";
    } // if
    cout << endl;
} // reset

void Potato::countdown() {
    currentTick--;
    if ( currentTick == 0 ) {
        _Throw( Explode() );
    } // if
} // countdown