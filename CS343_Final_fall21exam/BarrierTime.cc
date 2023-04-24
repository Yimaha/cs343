#include <iostream>
using namespace std;
#include "AutomaticSignal.h"
#include "MPRNG.h"

MPRNG mprng;

_Monitor Barrier {
  const unsigned int N;   // group size
  unsigned int count = 0, // blocked tasks (< N)
      total = 0,          // value sum
      groupno = 0;        // group number

  // VARIABLES FOR EACH IMPLEMENTATION, #ifdef NOT REQUIRED

  // ANY HELPER MEMBERS
public:
  struct Result {
    unsigned int total, groupno;
  };
  Barrier(unsigned int N) : N(N) {
    cout << N << " " << endl;
    // INITIALIZATION FOR EACH IMPLEMENTATION, #ifdef NOT REQUIRED
  } // Barrier::Barrier

#ifdef EXT

private:
  unsigned int returned = 0;
  unsigned int tempTotal = 0;
  unsigned int failedCount = 0;

public:
  void timeout() {
    osacquire(cout) << "TIMEOUT " << count << " waiting" << endl;
  } // Barrier::timeout

  Result block(unsigned int value) {
    if (count == N) _Accept(block);
    if (count == 0) {
      osacquire(cout) << "GROUP " << groupno + 1 << endl;
    }
    tempTotal += value;
    count += 1;

    if (count < N) {
      while (count < N) {
        _Accept(timeout) { break; }
        or _When(failedCount == 0) _Accept(block){};
      }
    } else {
      total = tempTotal;
      tempTotal = 0;
    }

    unsigned int returnGroupno;
    unsigned int returnTotal;
    if (total == 0) { // timout happened
      returned += 1;
      returnGroupno = groupno;
      returnTotal = 0;
      if (returned == 1) {
        tempTotal = 0;
        total = 0;
        failedCount = count;
        count = 0;
      }
      if (returned == failedCount) {
        returned = 0;
        groupno += 1;
        failedCount = 0;
      }
      timeout();
    } else {
      returned += 1;
      returnGroupno = groupno;
      returnTotal = total;
      if (returned == N) {
        count = 0;
        returned = 0;
        total = 0;
        groupno += 1;
      }
    }
    return {returnTotal, returnGroupno + 1};
  } // Barrier::block

#elif INT
private:
  uCondition bench;
  uCondition waitingbench;
  unsigned int finished = 0;

public:
  void timeout() {
    osacquire(cout) << "TIMEOUT " << count << " waiting" << endl;

    while (!bench.empty()) {
      bench.signalBlock();
    }
  } // Barrier::timeout

  Result block(unsigned int value) {

    if (finished != 0) {
      waitingbench.wait();
    }

    if (count == 0) {
      osacquire(cout) << "GROUP " << groupno + 1 << endl;
    }

    total += value;
    count += 1;

    if (count < N) {
      waitingbench.signal();
      bench.wait();
    }

    unsigned int returnGroupno;
    unsigned int returnTotal;
    if (count < N) {
      // waken due to timout
      finished += 1;
      returnGroupno = groupno;
      returnTotal = 0;
      // cout << "count " << count << "finished" << finished << endl;;
      if (count == finished) {
        finished = 0;
        count = 0;
        groupno += 1;
        total = 0;
        // signalling is handled by timeout for safety
      }
    } else {
      finished += 1;
      returnGroupno = groupno;
      returnTotal = total;
      // cout << "count " << count << "finished" << finished << endl;;

      if (count == finished) {
        finished = 0;
        count = 0;
        groupno += 1;
        total = 0;
        waitingbench.signal();
      } else {
        bench.signal();
      }
    }
    return {returnTotal, returnGroupno + 1};
  } // Barrier::block
#elif INTB
private:
  unsigned int finished = 0;
  int ticketRound = 0;
  int serving = 0;
  bool timeoutHappened = false;
  // ***********************
  // DO NOT CHANGE THIS CODE
  // ***********************
  uCondition __INTB_bench__;
  void wait() {
    __INTB_bench__.wait();     // wait until signalled
    while (mprng() % 5 == 0) { // multiple bargers allowed
      _Accept(block) {         // accept barging callers
      }
      _Else { // do not wait if no callers
      }       // Accept
    }         // while
  }
  void signalAll() {
    while (!__INTB_bench__.empty())
      __INTB_bench__.signal(); // drain condition
  }

public:
  void timeout() {
    // WRITE THIS MEMBER FOR INTERNAL SCHEDULING JAVA-STYLE
    osacquire(cout) << "TIMEOUT " << count << " waiting" << endl;

    timeoutHappened = true;
    serving += count;
    signalAll();
  } // Barrier::timeout

  Result block(unsigned int value) {
    // WRITE THIS MEMBER FOR INTERNAL SCHEDULING JAVA-STYLE
    ticketRound += 1;
    unsigned int ticket = ticketRound;

    while (count == N && serving < ticket) {
      wait();
    }
    if (count == 0) {
      osacquire(cout) << "GROUP " << groupno + 1 << endl;
    }
    total += value;
    count += 1;
    unsigned int returnGroupno;
    unsigned int returnTotal;
    if (count < N) {
      while ((count < N || timeoutHappened) && serving < ticket) {
        wait();
      }
    }
    returnGroupno = groupno;
    returnTotal = count < N ? 0 : total;
    finished += 1;
    if (!timeoutHappened) {
      signalAll();
      serving += 1;
    }
    if (finished == count) {
      total = 0;
      groupno += 1;
      count = 0;
      finished = 0;
      timeoutHappened = false;
    }

    return {returnTotal, returnGroupno + 1};
  } // Barrier::block
#elif AUTO
private:
  bool timeoutHappened = false;
  unsigned int tempGroupNumber = 0;
  unsigned int finished = 0;

public:
  AUTOMATIC_SIGNAL;
  void timeout() {
    osacquire(cout) << "TIMEOUT " << count << " waiting" << endl;

    // WRITE THIS MEMBER FOR IMPLICIT (AUTOMATIC) SIGNALLING
    timeoutHappened = true;
    EXIT();
  } // Barrier::timeout

  Result block(unsigned int value) {
    if (count == N) {
      WAITUNTIL(count < N, , ); // avoid barging
    }
    if (count == 0) {
      osacquire(cout) << "GROUP " << groupno + 1 << endl;
    }
    total += value;
    count += 1;
    WAITUNTIL(((count == N) || timeoutHappened), , );
    unsigned int returnGroupno;
    unsigned int returnTotal;
    if (timeoutHappened) {
      finished += 1;
      returnGroupno = groupno;
      returnTotal = 0;
      if (finished == count) {
        finished = 0;
        count = 0;
        total = 0;
        groupno += 1;
        timeoutHappened = false;
      }
    } else {
      finished += 1;
      returnGroupno = groupno;
      returnTotal = total;
      if (finished == count) {
        finished = 0;
        count = 0;
        total = 0;
        groupno += 1;
        timeoutHappened = false;
      }
    }
    // WRITE THIS MEMBER FOR IMPLICIT (AUTOMATIC) SIGNALLING
    EXIT();
    return {returnTotal, returnGroupno + 1};
  } // Barrier::block
#else
#error Unknown monitor implementation
#endif

private:
#ifdef INTB

public:
#endif
};

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
      Barrier::Result value = b.block(i);
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
