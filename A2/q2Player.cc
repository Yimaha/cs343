
#include "q2Player.h"
#include <iostream>
using namespace std;

Player::Player( PRNG &prng, int id, Potato &potato ) : prng( &prng ), id( id ), potato( &potato ) {}

void Player::main() {
    suspend(); // should be called from start, and immediately suspend
    for ( ;; ) {
        try {
            _Enable {
                cout << id;
                try {
                    potato->countdown(); // when tossed, count down potato
                } catch ( Potato::Explode & ) {
                    // if exploded, we kill the current player regardless
                    cout << " is eliminated" << endl;
                    if ( id != Player::umpire->getId() ) {
                        // if not umpire we tell empire to kill this player
                        _Resume Terminate( *this ) _At *Player::umpire;
                        Player::umpire->terminate();
                    } else {
                        // if it is umpire, we start election
                        cout << "E " << id;
                        _Resume Election( partner[1] ) _At *partner[1];
                        partner[1]->vote();
                    }  // if
                } // try
                cout << " -> "; // potato did not explode, continue
                unsigned int left = prng->operator()( 0, 1 ); // left is 0, right is 1
                if ( left == 0 ) {
                    partner[0]->toss();
                } else {
                    partner[1]->toss();
                } // if
            } // _Enable
        } catch ( Terminate &terminate ) {
            bool gameover = handleTerminate( terminate ); // it should only be caught by umpire
          // if handleTermination return true, the game is over as only 1 player left
          if ( gameover ) {
            cout << id << " wins the Match!" << endl;
            break;
          } // if
        } catch ( Election &election ) {
            handleElection( election );
        } // try
    } // for
} // main

void Player::start( Player &lp, Player &rp ) {
    partner[0] = &lp;
    partner[1] = &rp;
    resume();
} // start

void Player::toss() {
    resume(); // since you now have the potato, you resume
} // toss

int Player::getId() { return id; }

void Player::terminate() {
    resume(); // should just call resume, handler is within the main 
} // terminate

bool Player::handleTerminate( Terminate &terminate ) {
    bool gameover = false; // default to false
    Player *victim = &terminate.victim;
    Player *lp = terminate.victim.partner[0];
    Player *rp = terminate.victim.partner[1];
    if ( lp == rp ) { // last person standing
        gameover = true;
    }
    // fix the circle
    lp->partner[1] = rp;
    rp->partner[0] = lp;
    // delete the victim
    delete victim;
    // reset the potato
    potato->reset();
    // ready for restart
    cout << "U ";
    return gameover;
} // handleTerminate

void Player::vote() {
    resume(); // should just call resume, and exception handler will catch it in the loop
} // vote

void Player::handleElection( Election &election ) {
    if ( Player::umpire->getId() != id ) {
        // if it is not umpire, pass exception to next person to right
        // and replace pointer if you have a higher id
        cout << " -> " << id;
        if ( election.player->getId() < id ) {
            election.player = this;
        } // if
        _Resume election _At *partner[1];
        partner[1]->vote();
    } else {
        // we went around the circle and back
        // and we are ready to change umpire, then terminate
        Player::umpire = election.player;
        cout << " : umpire " << election.player->id << endl;
        _Resume Terminate( *this ) _At *Player::umpire;
        Player::umpire->terminate();
    } // if
} // handleElection