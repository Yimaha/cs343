// Purpose: copy file
//
// Command syntax:
//   $ ./a.out [ size (>= 0) | 'd' (default 20) [ code (>= 0) | 'd' (default 5)
//   [ input-file [ output-file ] ] ] ]
// all parameters are optional. Defaults: size=>20, code=>5, input=>cin,
// output=>cout
//
// Examples:
//   $ ./a.out 34
//   $ ./a.out 34 0
//   $ ./a.out 34 0 in
//   $ ./a.out 34 0 in out

#include "q3fmtcode.h"
#include <fstream>
#include <iostream>
#include <string>    // strtol
using namespace std; // direct access to std

void detectExtraneous( string preString, long unsigned int i, string line) {
    cout << preString; // always print preString
    if ( i != line.size() - 2 && i != line.size() - 1) { // if i is either at last char or \n
        cout << " - extraneous characters '" << line.substr( i + 1, ( line.size() - ( i + 1 ) - 1 ) ) << "'";
    } // if
    cout << endl; // always endl
}

int main( int argc, char *argv[] ) {
    istream *infile = &cin; // default value

    try {
        switch ( argc ) {
        case 2:
            try { // open input file first as output creates file
                infile = new ifstream( argv[1] );
            } catch ( uFile::Failure & ) { // open failed ?
                cerr << "Error! Could not open input file \"" << argv[3] << "\"" << endl;
                throw 1;
            }   // try
        case 1: // defaults
            break;
        default: // wrong number of options
            throw 1;
        } // switch
    } catch ( ... ) {
        cerr << "Usage: " << argv[0] << " [ input-file ]" << endl;
        exit( EXIT_FAILURE ); // TERMINATE
    }                         // try
    // cout << "size " << size << " code " << code << endl;

    *infile >> noskipws; // turn off white space skipping during input

    string line;
    for ( ;; ) { // copy input file to output file
        getline( *infile, line );
        if ( infile->fail() ) {
            break; // failed ?
        }
        if ( line.size() == 0 ) { // special handling for blank line
            cout << '\'' << line << "' : " << "Warning! Blank line." << endl; 
            continue;
        } // if
        cout << '\'' << line << "' : '";
        char ch;
        Fmtcode fmtcode;
        line += "\n"; // we append \n as a signal for end of line, thus avoiding a global flag var
        for ( string::size_type i = 0; i < line.size(); i++ ) {
            ch = line[i]; // save current processing char
            try {
                _Enable {
                    if ( ch != '\n' ) {
                        cout << ch;
                    } // if
                    fmtcode.next( ch );
                } // _Enable
            } catch ( Fmtcode::Match & ) {
                detectExtraneous("\' yes", i, line);
                break; // code should break immediately once detected yes, if extraneous we cut it off
            } 
            catch ( Fmtcode::Error & ) {
                detectExtraneous("\' no", i, line);
                break; // code should break immediately once detected no, if extraneous we cut it off
            } // try 
        }

    } // for

    if ( infile != &cin ) {
        delete infile;
    } // close file, do not delete cin!
} // main