// COMPILE COMMAND:
// u++ -g grammar.cc -o grammar

/* SAMPLE TEST DATA (valid and invalid)
(x)
(xxx)
(x[abab])
(x[ab(x)])
(x[ab(x)(x)ab(x[ab]x)])
(a)
(xxa)
([])
(x[ab[x]])
(x[ab(x)(x)ab(x(ab)x)])
*/

#include <fstream>
#include <iostream>
using namespace std;

_Coroutine Grammar {
public:
  _Event Match{}; // characters form a valid string in the language
  _Event Error{}; // last character results in string not in the language
private:
  char ch; // character passed by cocaller
  // YOU ADD MEMBERS HERE
  bool is(char c) { return c == ch; }
  void check(char c) {
    if (!is(c)) {
      throw Error();
    }
    suspend();
  }
  void checkXs() {
    check('('); // read '('
    check('x'); // read 'x'
    while (ch != ')') { // reading X
      checkX();
    }
    check(')'); // read ')'
  }

  void checkX() {
    if (is('x')) {
      suspend();
    } else {
      checkABs();
    }
  }

  void checkABs() {
    check('['); // read '['
    check('a'); // read 'a'
    check('b'); // read 'b'
    while (ch != ']') {
      checkAB();
    }
    check(']'); // read ']'
  }

  void checkAB() {
    if (is('a')) {
      suspend(); // read 'a'
      if (!is('b')) {
        throw Error();
      }
      suspend(); // read 'b'
    } else {
      checkXs();
    }
  }
  void main() {
    try {
      checkXs();
      throw Match();
    } catch (Error) {
      _Resume Error() _At resumer();
    } catch (Match) {
      _Resume Match() _At resumer();
    }

  } // Grammar::main
public:
  void next(char c) {
    ch = c;
    resume();
  } // Grammar::next
};  // Grammar

void checkRemaining(string line, int i) {
    if(i < line.size() && line[i] != '\n') {
        cout << " Extra characters \'" ;
        for (; i < line.size(); i++) {
            if(line[i] != '\n') {
                cout << line[i];
            }
        }
        cout << "'" << endl;
    } else {
        cout << "\n";
    }
}

int main() {
  string line;

  for (;;) {
    getline(cin, line); // read line
    if (cin.fail()) {
      break;
    } // EOF ?
    if (line.size() == 0) {
      continue;
    }
    if (isspace(line[0])) {
      continue;
    }
    // ignore empty line ?
    cout << "'" << line << "' : '";
    // YOU CREATE COROUTINE, SEND CHARACTERS TO IT, HANDLE EXCEPTIONS, AND PRINT
    // ANY EXTRA CHARACTERS
    line += "\n"; // add end of line character for easier identification if anything is wrong
    Grammar grammer;
    for (string::size_type i = 0; i < line.size(); i++) {
      char ch = line[i];
      try {
        _Enable {
          if (ch != '\n') {
            cout << ch;
          } // if
          grammer.next(ch);
        }
      } catch (Grammar::Match) {
        cout << "' : yes.";
        checkRemaining(line, i+1);
        break;
      } catch (Grammar::Error) {
        cout << "' ’: no.";
        checkRemaining(line, i+1);
        break;
      }
    }
    
  } // for
} // main
