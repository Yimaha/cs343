#ifndef __FMT_CODE__
#define __FMT_CODE__ 1
_Coroutine Fmtcode {
    char ch; // character passed by cocaller
    // YOU ADD MEMBERS HERE
    bool is( char target );
    bool isDigits();
    bool isLength();
    bool isSpecifier();
    bool isStar();
    bool isDot();
    void startWidthOrPrecision();
    void startLengthOrSpecifier();
    void checkWidth();
    bool checkPrecision();
    void checkLengthAndSpecifier( char specialExclusion[] = {}, int length = 0 );
    void checkSpecifier( char specialExclusion[] = {}, int length = 0 );
    void main(); // coroutine main
  public:
    _Event Match{}; // last character match
    _Event Error{}; // last character invalid
    void next( char c ) {
        ch = c;   // communicate input
        resume(); // activate
    }
};

#endif