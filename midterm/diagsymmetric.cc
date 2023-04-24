// COMPILE COMMANDS:
// $ u++ -g -multi diagsymmetric.cc -DCFOR
// $ u++ -g -multi diagsymmetric.cc -DACTOR
// $ u++ -g -multi diagsymmetric.cc -DTASK

/* SAMPLE TEST DATA (vald and invalid)
2
3 2
2 3

2
3 2
1 3

4
7 2 3 4
2 7 4 5
3 4 7 6
4 5 6 7

4
7 2 3 2
2 7 4 5
3 4 7 6
4 5 6 7

5
-1  2  3  4  5
 2 -1  4  5  6
 3  4 -1  6  7
 4  5  6 -1  8
 5  6  7  8 -1

5
-1  2  3  4  5
 2  0  4  5  6
 3  4 -1  6  7
 4  5  6 -1  8
 5  6  7  8 -1

20
1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1

20
1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0
2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0
0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0
0 2 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1
*/

#if defined( CFOR )
#include <uCobegin.h>
#elif defined( ACTOR )
#include <uActor.h>
#endif
#include <iostream>
using namespace std;

_Event NotDS {};										// not diagonally symmetric
void diagSymmetricCheck( const int (*M)[20], int row, int cols, uBaseTask & pgmMain ) {
	// YOU WRITE THIS CODE
    if (M[row][row] != M[0][0]) {
        _Resume NotDS{} _At pgmMain;
    }
    for (int i = row; i < cols; i++) {
        if(M[row][i] != M[i][row]) {
            _Resume NotDS{} _At pgmMain;
        }
    }
} // diagSymmetricCheck
 
#if defined( ACTOR )

struct WorkMsg : public uActor::Message {
    const int (*M)[20];
    int row;
    int cols;
    uBaseTask * pgmMain;
	// WRITE THIS TYPE
    WorkMsg(const int (*M)[20], int row, int cols, uBaseTask & pgmMain): 
        Message(uActor::Delete),M(M),row(row), cols(cols), pgmMain(&pgmMain) {}
}; // WorkMsg

_Actor DiagSymmetric {
	Allocation receive( Message & msg ) {
		Case(WorkMsg, msg) {
            diagSymmetricCheck(msg_d->M, msg_d->row, msg_d->cols, *msg_d->pgmMain);
            return uActor::Finished;
        }
        return uActor::Nodelete;
	} // DiagSymmetric::receive
}; // DiagSymmetric

#elif defined( TASK )

_Task DiagSymmetric {									// check row of matrix
  public:
	_Event Stop {};										// concurrent exception
  private:
	// YOU ADD MEMBERS HERE
    const int (*M)[20];
    int row;
    int cols;
    uBaseTask & pgmMain;
	void main() {
		// YOU WRITE THIS MEMBER
        yield(rand() % 100);
        try {
            _Enable {
                diagSymmetricCheck(M, row, cols, pgmMain);
            }
        } catch (Stop &) {
            osacquire(cout) << "stopped " << row << endl;
        }
	} // DiagSymmetric::main
  public:
	DiagSymmetric( const int (*M)[20], int row, int cols, uBaseTask & pgmMain ): M(M), row(row), cols(cols), pgmMain(pgmMain) {
		// YOU WRITE THIS MEMBER
	} // DiagSymmetric::DiagSymmetric
}; // DiagSymmetric

#endif

int main() {
	int N;
	cin >> N;

	int M[N][20], r, c;									// maximum 20 columns
	bool notDS = false;

	for ( r = 0; r < N; r += 1 ) {						// read/print matrix
		for ( c = 0; c < N; c += 1 ) {
			cin >> M[r][c];
			cout << M[r][c] << ", ";
		} // for
		cout << endl;
	} // for

	uProcessor p;										// extra processor

	// YOU WRITE THIS CODE
    uBaseTask * pgmMain = &uThisTask();

#if defined( CFOR )
	// YOU WRITE THIS CODE
    try {
	COFOR(i, 0, r,
        diagSymmetricCheck(M, i, c, *pgmMain);
	);
	_Enable {}	
    } catch (NotDS &) {
        notDS = true;
    }
									  // MUST APPEAR HERE
	// YOU WRITE THIS CODE

#elif defined( ACTOR )

	// YOU WRITE THIS CODE
    try {
        uActor::start();
        DiagSymmetric actors[r];
        for (int i = 0; i < r; i++) {
            actors[i] | *new WorkMsg(M, i, c, *pgmMain);
        }
        uActor::stop();
        _Enable {}										  // MUST APPEAR HERE
    } catch (NotDS &) {
        notDS = true;
    }

	// YOU WRITE THIS CODE

#elif defined( TASK )

	// YOU WRITE THIS CODE
	// _Enable {}
	// delete ...
	// _Enable {}
	// YOU WRITE THIS CODE

    int j = N-1;
    DiagSymmetric * tasks[r];
    try {
        for (int i = 0; i < r; i++) {
            tasks[i] = new DiagSymmetric(M, i, c, *pgmMain);
        }
        _Enable {
            for (; j >= 0; j--) {
                _Enable{};
                delete tasks[j];
                _Enable{}; // poll for async exception
            }
        }

    }_CatchResume(NotDS &) {
        if (!notDS) {
            for (int i = j - 1; i >= 0; i--) {
                _Resume DiagSymmetric::Stop() _At *tasks[i];
            }
            notDS = true;
        }
    }
#else
	#error unknown concurrency approach specified on command line, e.g., need -DTASK
#endif

	cout << "matrix is" << (notDS ? " not " : " ") << "diagonally symmetric" << endl;
} // main
