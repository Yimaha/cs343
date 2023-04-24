#include "PRNG.h"
#include "q2Player.h"
#include "q2Potato.h"
#include <cstdlib> // access: rand, srand
#include <cstring> // access: strcmp
#include <iostream>
using namespace std;
#include <unistd.h> // access: getpid

Player *Player::umpire = nullptr;

int main( int argc, char *argv[] ) {
    int games = 5, players = -1, seed = getpid(); // default values
    bool randPlayers = true;
    try {
        switch ( argc ) {
        case 4:
            if ( strcmp( argv[3], "d" ) != 0 ) { // default pid
                seed = stoi( argv[3] );
                if ( seed <= 0 ) throw 1;
            } // if
        case 3:
            if ( strcmp( argv[2], "d" ) != 0 ) { // default random(2-10)
                players = stoi( argv[2] );
                randPlayers = false;
                if ( players < 2 ) throw 1;
            } // if
        case 2:
            if ( strcmp( argv[1], "d" ) != 0 ) { // default 5
                games = stoi( argv[1] );
                if ( games < 0 ) throw 1;
            } // if
        case 1:
            break; // use all defaults
        default:
            throw 1;
        } // switch
    } catch ( ... ) {
        cerr << "Usage: " << argv[0] << " [ games (>=0) | 'd' (default 5) [ players (>=2) | 'd' (random 2-10) [ seed (>0) | 'd' (random) ] ] ]" << endl;
        exit( EXIT_FAILURE );
    } // try
    PRNG prng_1 = PRNG( seed ); // for main
    PRNG prng_2 = PRNG( seed ); // for potato
    PRNG prng_3 = PRNG( seed ); // for player

    for ( int i = 0; i < games; i++ ) {
        if ( randPlayers ) { 
            players = prng_1( 2, 10 ); // if amount of player is not defined we randomize one
        } // if 
        cout << players << " players in the game" << endl;

        Potato potato = Potato( prng_2 ); 

        // create a array full of pointer to Players
        // note that the reason why we are putting pointers is because in the assignment
        // instruction, when potato explode the player also needs to be deleted
        Player *circle[players]; 
        int randswap = prng_1( players - 1 ); // decide randomly which character to swap with 0
        for ( int i = 0; i < players; i++ ) {
            if ( i == randswap ) {
                circle[i] = new Player( prng_3, 0, potato );
                Player::umpire = circle[i];
            } else if ( i == 0 ) {
                circle[i] = new Player( prng_3, randswap, potato );
            } else {
                circle[i] = new Player( prng_3, i, potato );
            } // if
        } // for
        // loop to call start
        for ( int i = 0; i < players; i++ ) {
            if ( i == 0 ) {
                circle[i]->start( *circle[players - 1], *circle[i + 1] );
            } else if ( i == players - 1 ) {
                circle[i]->start( *circle[i - 1], *circle[0] );
            } else {
                circle[i]->start( *circle[i - 1], *circle[i + 1] );
            } // if
        } // for
        // now the circle is set up, we are ready to begin and toss potato to umpire
        cout << "U ";
        Player::umpire->toss();
        // once program returned to main, there should only be the umpire left, thus we just delete the umpire
        delete Player::umpire;
        Player::umpire = nullptr;
        if ( i != games - 1 ) {
            cout << "\n\n"; // the last game don't print new line
        } // if
    } //for 
} // main