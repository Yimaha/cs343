
#include "q3pc.h"
#include "MPRNG.h"
#include <iostream>
#include <string>
#include <unistd.h> // access: getpid
using namespace std;

MPRNG mprng( getpid() ); // we initilize the mprng to a value according to pid

void Consumer::main() {
    while ( true ) // change later lol
    {
        yield( mprng( delay ) ); // we randomly wait a certain seconds
        int result = buffer->remove(); // call the remove 
        if ( result != sentinel ) { // if did got sentinel
            accumulate += result; // we accumulate
        } else { // if got sentinel
      break; // we kill the task
        } // if
    } // while
}
// in destructor, we copy the accumulate to the address passed to us
Consumer::~Consumer() { *sum = accumulate; } 

void Producer::main() {
    for ( int i = 0; i < produce; i++ ) {
        yield( mprng( delay ) ); // we randomly wait a certain seconds
        buffer->insert( i + 1 ); // we start inserting
    } // if
}