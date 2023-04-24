// COMPILE COMMAND:
// u++ -g figure8.cc -o figure8

/* SAMPLE TEST DATA
use command-line argument
*/

#include <iostream>
using namespace std;

_Coroutine Umpire;

_Coroutine Player {
	// YOU ADD MEMBERS HERE
    unsigned int v;
	void main(); // MEMBER BODY MUST BE DEFINED AFTER Umpire (SEE BELOW)
  protected:
	_Event Rm {											// remove exception
	  public:
		Player * vict;
		Rm( Player * vict ) : vict( vict ) {}
	}; // Rm

	Umpire * umpire;
										// player identity
	Player * cycle1;									// A player cycle

	void unlink( Player *&p ) {							// remove player from cycle
		((Player *)&p->resumer())->cycle1 = p->cycle1;
	} // Player::unlink
  public:
    unsigned int id;
	Player( Umpire * umpire, unsigned int id, Player * cycle1 ) :
			umpire( umpire ), id( id ), cycle1( cycle1 ) {}
	
    void toss( unsigned int v ) {
        Player::v = v;
        resume();
	} // Player::toss
}; // Player

_Coroutine Umpire : public Player {
	Player * cycle2;									// B player cycle
	unsigned int numPlayers;

	void helper( Player *& cycle, char cycleName ) {
		_Enable {
			unsigned int vict = rand() % numPlayers ;
			if ( cycle != this ) {
				cout << cycleName << " -" << vict << " :";
				cycle->toss( vict );					// toss can change cycle
				cout << endl;
			} // if
			if ( cycle == this ) {
				cout << cycleName << " empty" << endl;
			} // if
		} // _Enable
	} // helper

	// YOU ADD MEMBERS HERE
    bool shouldStop( ) {
        return cycle1 == this && cycle2 == this;
    }
	void main() {
        for(;;) {
            try {
                helper(cycle1, 'A');
            } catch (Rm & rm) {
                Player *victim = rm.vict;
                unlink(victim);
                delete victim;
                cout << endl;
                if(cycle1 == this) {
                    cout << "A empty" << endl;
                }
            }

            try {
                helper(cycle2, 'B');
            } catch (Rm & rm) {
                Player *victim = rm.vict;
                swap(cycle1, cycle2);
                unlink(victim);
                swap(cycle2, cycle1);
                delete victim;
                cout << endl;
                if(cycle2 == this) {
                    cout << "B empty" << endl;
                }                
            }
          if(shouldStop()) { break; }
            
        }

	} // Player::main
  public:
	Umpire( unsigned int numPlayers ) :
			Player( nullptr, numPlayers, nullptr ), numPlayers( numPlayers ) {}
	void close( Player * cyc1, Player * cyc2 ) { cycle1 = cyc1; cycle2 = cyc2; }
}; // Umpire

void Player::main() {									// MUST BE DEFINED HERE TO KNOW UMPIRE IS COROUTINE
	// YOU WRITE THIS MEMBER
    for (;;) {
        cout << " ";
        if (id == v) {
            cout << "-";
            _Resume Rm(this) _At *Player::umpire;
        } 
        cout << id;
        cycle1->toss(v);
    }
} // Player::main

int main( int argc, char * argv[] ) {
	srand( getpid() );

	int numPlayers = 4;

	try {												// process command-line arguments
		switch ( argc ) {
		  case 2:
			numPlayers = stoi( argv[1] ); if ( numPlayers <= 0 ) throw 1;
		} // switch
	} catch( ... ) {
		cout << "Usage: " << argv[0] << " [ numPlayers (> 0) ]" << endl;
		exit( 1 );
	} // try

	Umpire umpire( numPlayers );
	Player * A[numPlayers], * B[numPlayers];			// A & B player cycles
    for ( int i = 0; i < numPlayers; i++ ) {
        if(i == 0) {
            A[i] = new Player(&umpire, i, &umpire);
        } else {
            A[i] = new Player(&umpire, i, A[i-1]);
        }
    }

    for ( int i = 0; i < numPlayers; i++ ) {
        if(i == 0) {
            B[i] = new Player(&umpire, i, &umpire);
        } else {
            B[i] = new Player(&umpire, i, B[i-1]);
        }
    }
    umpire.close(A[numPlayers-1], B[numPlayers-1]);
    umpire.toss(0);
    // YOU INITIALIZE COROUTINES AND RESUME THE APPROPRIATE ONE
} // main
