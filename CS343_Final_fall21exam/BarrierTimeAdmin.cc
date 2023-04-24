#include <iostream>
#include <list>
using namespace std;
#include "MPRNG.h"
#include <queue>
#include <uFuture.h>
MPRNG mprng;

_Task Barrier {
public:
  struct Result {
    unsigned int total, groupno;
  };
  typedef Future_ISM<Result> Fresult; // future type
private:
  const unsigned int N;   // group size
  unsigned int count = 0, // blocked tasks (< N)
      total = 0,          // value sum
      groupno = 0;        // group number
                          // ADDITIONAL VARIABLES

  // ANY HELPER MEMBERS
  queue<unsigned int> vals;
  queue<Fresult> futures;
  unsigned int val;
  Fresult f;

public:
  Barrier(unsigned int N) : N(N) {
    // ADDITIONAL INITIALIZATION
  } // Barrier::Barrier

  void timeout() {
    // WRITE THIS MEMBER

  } // Barrier::timeout

  Fresult block(unsigned int value) {
    // WRITE THIS MEMBER
    val = value;
    Fresult future;
    f = future;
    return future;
  } // Barrier::block
private:
  void main() {
    // WRITE THIS MEMBER
    // PRINT TIMEOUT AND GROUP MESSAGE
    for (;;) {
      _Accept(~Barrier) { break; }
      or _Accept(block) {
        if (vals.empty()) {
          osacquire(cout) << "GROUP " << groupno + 1 << endl;
        }
        vals.push(val);
        futures.push(f);
        if (vals.size() == 4) {
          for (int i = 0; i < 4; i++) {
            total += vals.front();
            vals.pop();
          }
          for (int i = 0; i < 4; i++) {
            futures.front().delivery({total, groupno + 1});
            futures.pop();
          }
          groupno += 1;
        }
      }
      or _Accept(timeout) {
        osacquire(cout)  << "TIMEOUT " << vals.size() << " waiting" << endl;

        if (!vals.empty()) {
          while (!vals.empty()) {
            vals.pop();
          }
          while (!futures.empty()) {
            futures.front().delivery({0, groupno + 1});
            futures.pop();
          }
          groupno += 1;
        }
      }
    }
  } // Barrier::main
};  // Barrier

// ****************************
// DO NOT CHANGE ANY CODE BELOW
// ****************************

_Task Worker {
  unsigned int id;
  Barrier &b;
  unsigned int times;

  void main() {
    unsigned int total = 0, timeouts = 0;

    yield(mprng() % 10); // do not start at once
    for (unsigned int i = 1; i <= times; i += 1) {
      Barrier::Fresult fvalue = b.block(i);
      yield(mprng() % 10); // asynchronous work
      Barrier::Result value = fvalue();
      yield(mprng() % 10); // time slice ?
      if (value.total == 0) {
        timeouts += 1;
      } else {
        total += value.total;
      } // if
      osacquire(cout) << "W" << id << " group " << value.groupno << " totals "
                      << value.total << ',' << total << endl;
      yield(mprng() % 10); // process value
    }                      // for
    osacquire(cout) << "Worker " << id << " finish: total " << total
                    << " timeouts " << timeouts << endl;
  } // Barrier::main
public:
  Worker(unsigned int id, Barrier &b, unsigned int times)
      : id(id), b(b), times(times) {}
}; // Worker

volatile bool stop = false;

_Task Timer {
  Barrier &b;
  unsigned int group;

  void main() {
    for (; !stop;) {
      yield(mprng() % (group * 20)); // trigger less for larger groups
      b.timeout();
    } // for
    osacquire(cout) << "Timer finish" << endl;
  } // Timer::main
public:
  Timer(Barrier & b, unsigned int group) : b(b), group(group) {}
}; // Timer

static long int convert(char *argv) { // convert C string to integer
  char *endptr;
  errno = 0;
  long int val = strtol(argv, &endptr, 10); // attempt conversion
  if (errno == ERANGE)
    throw std::out_of_range("");
  if (endptr == argv || // conversion failed, no characters generated
      *endptr != '\0')
    throw std::invalid_argument(""); // not at end of argv ?
  return val;
} // convert

int main(int argc, char *argv[]) {
  enum { Dworkers = 6, Dgroup = 4, Dtimes = 30 };
  int nworkers = Dworkers, group = Dgroup, times = Dtimes, seed = getpid();

  try {
    switch (argc) {
    case 5:
      if (strcmp(argv[4], "d") != 0) { // user default ?
        if (seed = convert(argv[4]), seed < 1)
          throw 1;
      } // if
        // FALL THROUGH
    case 4:
      if (strcmp(argv[3], "d") != 0) { // user default ?
        if (times = convert(argv[3]), times < 1)
          throw 1;
      } // if
        // FALL THROUGH
    case 3:
      if (strcmp(argv[2], "d") != 0) { // user default ?
        if (group = convert(argv[2]), group < 1)
          throw 1;
      } // if
        // FALL THROUGH
    case 2:
      if (strcmp(argv[1], "d") != 0) { // user default ?
        if (nworkers = convert(argv[1]), nworkers < 1)
          throw 1;
      } // if
      if (nworkers < group)
        throw 1;
      // FALL THROUGH
    case 1: // defaults
      break;
    default: // wrong number of options
      throw 1;
    } // switch
  } catch (...) {
    cerr << "Usage: " << argv[0]
         << " [ workers (> 0 & <= group) | 'd' (default " << Dworkers << ")"
         << " [ group (> 0) | 'd' (default " << Dgroup << ")"
         << " [ times (> 0) | 'd' (default " << Dtimes << ")"
         << " [ seed (> 0) | 'd' (default random)"
         << " ] ] ] ]" << endl;
    exit(EXIT_FAILURE); // TERMINATE!
  }                     // try
  // cout << nworkers << ' ' << group << ' ' << times << ' ' << seed << endl;
  cout << "seed " << seed << endl;

  mprng.set_seed(seed);
  uProcessor p; // parallelism

  Worker *workers[nworkers];
  Barrier b(group);

  Timer timer(b, group);
  for (int i = 0; i < nworkers; i += 1) {
    workers[i] = new Worker(i, b, times);
  } // for
  for (int i = 0; i < nworkers; i += 1) {
    delete workers[i];
  }            // for
  stop = true; // stop timer
} // main
