#ifndef __Player__
#define __Player__ 1

#include "PRNG.h"
#include "q2Potato.h"

_Coroutine Player {
    _Event Terminate {
      public:
        Player &victim; // delete player
        Terminate( Player & victim ) : victim( victim ) {}
    };
    _Event Election {
      public:
        Player *player; // highest player id seen so far
        Election( Player * player ) : player( player ) {}
    };
    Player *partner[2]; // left and right player
                        // YOU ADD MEMBERS HERE
    PRNG *prng;
    int id;
    Potato *potato;
    bool handleTerminate( Terminate & terminate );
    void handleElection( Election & election );
    void main();
    void vote();      // resume partner to vote
    void terminate(); // resume umpire
  public:
    static Player *umpire; // current umpire

    Player( PRNG & prng, int id, Potato &potato );
    void start( Player & lp, Player & rp ); // supply partners
    int getId();                            // player id
    void toss();                            // tossed potato (indicate you have potato now)
};

#endif