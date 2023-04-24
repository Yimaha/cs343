#include "q3fmtcode.h"
#include <iostream>
using namespace std; // direct access to std

// list of support function for checking frequently used pattern
bool Fmtcode::is( char target ) { return ( ch == target ); }

bool Fmtcode::isDigits() {
    return ( ch == '0' || ch == '1' || ch == '2' || ch == '3' || ch == '4' || ch == '5' || ch == '6' || ch == '7' || ch == '8' || ch == '9' );
}

bool Fmtcode::isLength() { return ( ch == 'h' || ch == 'l' || ch == 'j' || ch == 'z' || ch == 't' || ch == 'L' ); }

bool Fmtcode::isSpecifier() {
    return ( ch == 'd' || ch == 'i' || ch == 'u' || ch == 'o' || ch == 'x' || ch == 'X' || ch == 'f' || ch == 'F' || ch == 'e' || ch == 'E' ||
             ch == 'g' || ch == 'G' || ch == 'a' || ch == 'A' || ch == 'c' || ch == 's' || ch == 'p' || ch == 'n' );
}

bool Fmtcode::isStar() { return ( ch == '*' ); }

bool Fmtcode::isDot() { return ( ch == '.' ); }

void Fmtcode::startWidthOrPrecision() {
    if ( isDigits() || isStar() || isDot() ) { // either of these are okay
        checkWidth();
        bool hasPrecision = checkPrecision();
        if ( !hasPrecision ) {
            char arr[1] = { 'n' }; // since precision is not provided, only n is invalid
            checkLengthAndSpecifier( arr, 1 );
            checkSpecifier( arr, 1 );
        }                                // if
        char arr[3] = { 'n', 'p', 'c' }; // since precision is provided, n, p, c are invalid
        checkLengthAndSpecifier( arr, 3 );
        checkSpecifier( arr, 3 );
        // note the program should never reach here
    } // if
}

void Fmtcode::startLengthOrSpecifier() {
    // if start with either length or specifier no additional handle required
    if ( isLength() || isSpecifier() || is( '\n' ) ) { // note that we could get \n here, as a special case
        checkLengthAndSpecifier();
        checkSpecifier();
    } // if
}

// check if any of the width pattern happens
void Fmtcode::checkWidth() {
    if ( isDigits() ) {
        suspend();
        for ( ;; ) {
            if ( !isDigits() ) { break; } // if
            suspend();
        } // for
    } else if ( isStar() ) {
        suspend();
    } // if
}

// check if any precision pattern exists
bool Fmtcode::checkPrecision() {
    if ( isDot() ) {
        suspend();
        if ( isDigits() ) {
            for ( ;; ) {
                suspend();
                if ( !isDigits() ) { break; } // if
            }                                 // for
        } else if ( isStar() ) {
            suspend();
        } // if
        return true;
    } // if
    return false;
}

// check if any length + specifier pattern exists(legal only)
void Fmtcode::checkLengthAndSpecifier( char specialExclusion[], int length ) {
    if ( isLength() ) {
        if ( is( 'h' ) ) {
            suspend();
            if ( is( 'h' ) ) { suspend(); } // if
        } else if ( is( 'l' ) ) {
            suspend();
            if ( !is( 'l' ) ) {
                // within this block, we init a particular pattern then merge additional specialExclusion if needed
                int localLength = 1;
                char arr[localLength + length] = { 'p' };
                copy( specialExclusion, specialExclusion + length, arr + localLength );
                checkSpecifier( arr, localLength + length );
            } // if
            suspend();
        } else if ( is( 'L' ) ) {
            suspend();
            // within this block, we init a particular pattern then merge additional specialExclusion if needed
            int localLength = 4;
            char arr[localLength + length] = { 'c', 's', 'p', 'n' };
            copy( specialExclusion, specialExclusion + length, arr + localLength );
            checkSpecifier( arr, localLength + length );

        } else {
            suspend();
        } // if
        // within this block, we init a particular pattern then merge additional specialExclusion if needed
        int localLength = 11;
        char arr[localLength + length] = { 'f', 'F', 'e', 'E', 'g', 'G', 'a', 'A', 'c', 's', 'p' };
        copy( specialExclusion, specialExclusion + length, arr + localLength );
        checkSpecifier( arr, localLength + length );
    } // if
}
// check if specifier pattern is detected
void Fmtcode::checkSpecifier( char specialExclusion[], int length ) {
    for ( int i = 0; i < length; i++ ) {
        if ( is( specialExclusion[i] ) ) { _Throw Error(); } // if
    }                                                        // for
    if ( isSpecifier() ) { _Throw Match(); }                 // if
    _Throw Error();
}

void Fmtcode::main() {
    try {
        // first level of processing, should only accept % and nothing else
        if ( is( '%' ) ) {
            suspend();
        } else {
            _Throw Error();
        } // if

        if ( is( '%' ) ) { // special case, no prefixes is allowed
            _Throw Match();
        } // if
        // check which specific type of string are we getting
        startWidthOrPrecision();
        startLengthOrSpecifier();
        // should never, ever reach here unless there is big problem (not part of the grammar)
        // for example %-2322d where negative number is not even part of the syntax
        _Throw Error();

    } catch ( Error & ) {
        _Resume Error() _At resumer();
        suspend();
    } catch ( Match & ) {
        _Resume Match() _At resumer();
        suspend();
    } // try
    // note code should never reach here, but if it does it just keep going
    for ( ;; ) {
        _Resume Error() _At resumer();
        suspend();
    }
}