#include "MPRNG.h"
#include "q3pc.h"
#include "q3buffer.h"
#include <cstdlib> // access: rand, srand
#include <cstring> // access: strcmp
#include <fstream>
#include <iostream>
#include <string>
using namespace std;

int main( int argc, char *argv[] ) {
    int processors = 1, delays = 0, bufferSize = 10, produce = 10, prods = 3, cons = 5;
    try {
        switch ( argc ) {
        case 7:
            if ( strcmp( argv[6], "d" ) != 0 ) { // default processor
                processors = stoi( argv[6] );
                if ( processors <= 0 ) throw 1;
            } // if
        case 6:
            if ( strcmp( argv[5], "d" ) != 0 ) { // default delay
                delays = stoi( argv[5] );
                if ( delays <= 0 ) throw 1;
            } // if
        case 5:
            if ( strcmp( argv[4], "d" ) != 0 ) { // default buffersizes
                bufferSize = stoi( argv[4] );
                if ( bufferSize <= 0 ) throw 1;
            } // if
        case 4:
            if ( strcmp( argv[3], "d" ) != 0 ) { // default pid
                produce = stoi( argv[3] );
                if ( produce <= 0 ) throw 1;
            } // if
        case 3:
            if ( strcmp( argv[2], "d" ) != 0 ) { // default random(2-10)
                prods = stoi( argv[2] );
                if ( prods <= 0 ) throw 1;
            } // if
        case 2:
            if ( strcmp( argv[1], "d" ) != 0 ) { // default 5
                cons = stoi( argv[1] );
                if ( cons <= 0 ) throw 1;
            } // if
        case 1:
            break; // use all defaults
        default:
            throw 1;
        } // switch
    } catch ( ... ) {
        cerr << "Usage: " << argv[0]
             << " [ Cons (> 0) | 'd' (default 5) [ Prods (> 0) | 'd' (default 3) [ Produce (> 0) | 'd' (default 10) [ BufferSize (> 0) | 'd' (default 10) [ "
                "Delay (> 0) | 'd' (default 8) [ Processors (> 0) | 'd' (default 1) ] ] ] ] ] ] "
             << endl;
        exit( EXIT_FAILURE );
    } // try
    uProcessor p[processors - 1] __attribute__(( unused )); // note this has on effect if processors = 1
    const unsigned int size = bufferSize; 
    BoundedBuffer<int> buffer = BoundedBuffer<int>(size); // initialize the buffer
    {
        // initialize the producer, start them
        Producer * producers[prods]; 
        for (int i = 0; i < prods; i++) {
            producers[i] = new Producer(buffer, produce, delays);
        } // for
        // to avoid racing, we have sum for each of them
        // similar to the matrix example
        int sum [cons];
        // initilize all the consumers and start them
        Consumer * consumers[cons]; 
        for (int i = 0; i < cons; i++) {
            consumers[i] = new Consumer(buffer, delays, SENTINEL, sum[i]);
        } // for
        // wait for all the producer to finish
        for (int i = 0; i < prods; i++) {
            delete producers[i];
        } // for
        // once all producer finished we start to insert SENTINEL values
        for (int i = 0; i < cons; i++) {
            buffer.insert(SENTINEL);
        } // for

        int total = 0;
        // wait for all consumer to finish and add their sum up
        for (int i = 0; i < cons; i++) {
            delete consumers[i];
            total += sum[i];
        } // for
        // print
        cout << "total: " <<  total << endl;
        // not sure if this should be printed
        // cout << "Total Producer/Consumer blocks in insert/remove: " << buffer.blocks() << endl;
    }

}