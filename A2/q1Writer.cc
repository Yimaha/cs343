

#include "q1Writer.h"
using namespace std;

void Writer::main() {
    int count = 0;
    try {
        _Enable {
            for ( ;; ) {
                *out << ch;
                count += 1;
                suspend();
            } // for
        } // _Enable
    } catch ( Eof & ) {
        *out << count << " characters" <<endl; // output the total amount of characters passed
    } // try
} // main