

#include "q1Reader.h"
using namespace std;

void Reader::main() {
    for ( ;; ) {
        *in >> ch;
      if ( in->fail() ) break;
        next->put( ch );
    } // for
    // once reached here the stream should be completely read
    _Resume Eof() _At *next;
    next->put( ' ' ); // irrelevant character
} // main