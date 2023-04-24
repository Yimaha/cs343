

#include "q1Filter.h"
using namespace std;
#include <iostream>

void FilterCase::main() {
    try {
        _Enable {
            for ( ;; ) {
                // go upper / lower depending on flag
                if ( upperCase && !isupper( ch ) ) {
                    ch = toupper( ch ); 
                } else if ( !upperCase && isupper( ch ) ) {
                    ch = tolower( ch );
                } // if
                next->put( ch );
                suspend();
            } // for
        } // _Enable
    } catch ( Eof &e ) {
        _Resume e _At *next;
        next->put( ' ' ); // irrelevant character
    } // try
} // main

void FilterNumber::main() {
    try {
        for ( ;; ) {
            _Enable {
                for ( int i = 0; i < 13; i++ ) {
                    if ( isalpha( ch ) ) {
                        int increment = number % 26;
                        // total of 4 cases when the increment / decrement exceed the range of characters (it should wrap around)
                        if ( isupper( ch ) && increment + ch > 'Z' && increment >= 0 ) {
                            ch = 'A' + increment + ch - 'Z' - 1;
                        } else if ( !isupper( ch ) && increment + ch > 'z' && increment >= 0 ) {
                            ch = 'a' + increment + ch - 'z' - 1;
                        } else if ( isupper( ch ) && increment + ch < 'A' && increment < 0 ) {
                            ch = 'Z' - ( 'A' - increment - ch - 1 );
                        } else if ( !isupper( ch ) && increment + ch < 'a' && increment < 0 ) {
                            ch = 'z' - ( 'a' - increment - ch - 1 );
                        } else {
                            ch += increment;
                        } // if
                    } // if
                    next->put( ch );
                    suspend();
                } // for
                // every 13 character the number increase by 1 (or decrease by 1 if number < 0)
                if ( number >= 0 ) {
                    number += 1;
                } else {
                    number -= 1;
                } // if
            } // _Enable
        } // for
    } catch ( Eof &e ) {
        _Resume e _At *next;
        next->put( ' ' ); // irrelevant character
    } // try
} // main


void FilterCapitalize::main() {
    try {
        _Enable {
            // since the first letter character must be upper case
            // we keep skipping every character until first letter is here
            while ( !isalpha( ch ) ) {
                next->put( ch );
                suspend();
            } // while
            // upper case first letter
            ch = toupper( ch );
            next->put( ch );
            suspend();
            for ( ;; ) {
                char lastChar = ch; // note that we have to keep track if we got '!', '.', or '?'
                next->put( ch );
                suspend();
                // if we did not, then it will just read again, but if it did:
                if ( lastChar == '!' || lastChar == '.' || lastChar == '?' ) {
                    if ( isspace( ch ) ) { // we already have 1 of the trigger, and it followed by a space
                        while ( isspace( ch ) ) { // we keep reading it while it still have space
                            next->put( ch );
                            suspend();
                        } // while
                        // at the end, if this is a character, it would be uppercased (ignores non letter)
                        ch = toupper( ch );
                        next->put( ch );
                        suspend();
                    } // if
                } // if 
            } // for 
        } // _Enable
    } catch ( Eof &e ) {
        _Resume e _At *next;
        next->put( ' ' ); // irrelevant character
    } // try
} // main