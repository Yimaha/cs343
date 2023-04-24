#ifndef __WRITER__
#define __WRITER__ 1

#include "q1Filter.h"
#include <iostream>
using namespace std;
_Coroutine Writer : public Filter {
    ostream * out;
    void main();
  public:
	Writer( ostream & o ): Filter(nullptr){
        out = &o;
    };
};

#endif