#ifndef __FILTER__
#define __FILTER__ 1

_Coroutine Filter {
  protected:
    _Event Eof{};     // no more characters
    Filter *next;     // next filter in chain
    unsigned char ch; // communication variable
  public:
    Filter( Filter * next ) : next( next ) {}
    void put( unsigned char c ) {
        ch = c;
        resume();
    }
};

_Coroutine FilterCase : public Filter {
    // YOU MAY ADD PRIVATE MEMBERS
    bool upperCase = false;
    void main();

  public:
    FilterCase( Filter * f, bool upperCase ) : Filter( f ), upperCase( upperCase ){};
};

_Coroutine FilterNumber : public Filter {
    // YOU MAY ADD PRIVATE MEMBERS
    int number;
    void main();

  public:
    FilterNumber( Filter * f, int number) : Filter( f ), number(number) {};
};

_Coroutine FilterCapitalize : public Filter {
    // YOU MAY ADD PRIVATE MEMBERS
    void main();

  public:
    FilterCapitalize( Filter * f ) : Filter( f ){};
};

#endif