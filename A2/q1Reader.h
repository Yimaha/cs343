#ifndef __READER__
#define __READER__ 1

#include "q1Filter.h"
#include <iostream>
using namespace std;

_Coroutine Reader : public Filter {
    // YOU MAY ADD PRIVATE MEMBERS
    istream *in;
    void main();

  public:
    Reader( Filter * f, istream & i ) : Filter( f ), in( &i ) { resume(); };
};

#endif