#ifndef Q2_TV
#define Q2_TV

#include "AutomaticSignal.h"
#include "BargingCheckVote.h"
#include "MPRNG.h"
#include "iostream"

_Monitor Printer;
extern MPRNG mprng;

#if defined( EXT ) // external scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
    unsigned int voters;         // track voters
    unsigned int group;          // track group size
    unsigned int numVotes;       // track current number of vote accumulated (useful
                                 // though not neccessary)
    unsigned int doneVoter;      // track how many tourist are gone
    unsigned int groupCounter;   // track the current group
    unsigned int numBlockedTour; // track number of people voted wating for full group
    unsigned int voteSum[3];     // track all the vote
    Printer &printer;            // DA PRINTER
#elif defined( INT )             // internal scheduling monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    uCondition bench;
    unsigned int voters;         // track voters
    unsigned int group;          // track group size
    unsigned int numVotes;       // track current number of vote accumulated (useful
                                 // though not neccessary)
    unsigned int doneVoter;      // track how many tourist are gone
    unsigned int groupCounter;   // track the current group
    unsigned int numBlockedTour; // track number of people voted wating for full group
    unsigned int voteSum[3];     // track all the vote
    Printer &printer;            // DA PRINTER
                                 // private declarations for this kind of vote-tallier
#elif defined( INTB )            // internal scheduling monitor solution with barging
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    // private declarations for this kind of vote-tallier
    uCondition bench;            // only one condition variable (variable name may be changed)
    unsigned int ticket;         // ticket counter
    unsigned int serving;        // current serving
    unsigned int voters;         // track voters
    unsigned int group;          // track group size
    unsigned int numVotes;       // track current number of vote accumulated (useful
                                 // though not neccessary)
    unsigned int doneVoter;      // track how many tourist are gone
    unsigned int groupCounter;   // track the current group
    unsigned int numBlockedVote; // track number of people block on trying to vote
    unsigned int numBlockedTour; // track number of people voted wating for full group
    unsigned int voteSum[3];     // track all the vote
    Printer &printer;            // DA PRINTER

    BCHECK_DECL;

    void wait();      // barging version of wait
    void signalAll(); // unblock all waiting tasks
#elif defined( AUTO )            // automatic-signal monitor solution
// includes for this kind of vote-tallier
_Monitor TallyVotes {
    AUTOMATIC_SIGNAL;
    unsigned int voters;         // track voters
    unsigned int group;          // track group size
    unsigned int numVotes;       // track current number of vote accumulated (useful
                                 // though not neccessary)
    unsigned int doneVoter;      // track how many tourist are gone
    unsigned int groupCounter;   // track the current group
    unsigned int numBlockedTour; // track number of people voted wating for full group
    unsigned int voteSum[3];     // track all the vote
    Printer &printer;            // DA PRINTER

    void block( unsigned int id );
    void unblock( unsigned int id );
    // private declarations for this kind of vote-tallier
#elif defined( TASK )            // internal/external scheduling task solution
_Task TallyVotes {
    // private declarations for this kind of vote-tallier
    unsigned int currentId;
    uCondition bench;            // only one condition variable (variable name may be changed)
    unsigned int voters;         // track voters
    unsigned int group;          // track group size
    unsigned int numVotes;       // track current number of vote accumulated (useful
                                 // though not neccessary)
    unsigned int doneVoter;      // track how many tourist are gone
    unsigned int groupCounter;   // track the current group
    unsigned int numBlockedTour; // track number of people voted wating for full group
    unsigned int voteSum[3];     // track all the vote
    Printer &printer;            // DA PRINTER

    void main(); // the intern you hired
    void doneVoting(); // clean up function for TASK case 
#else
#error unsupported voter type
#endif
    // common declarations
  public: // common interface
    _Event Failed{};
    struct Ballot {
        unsigned int picture, statue, giftshop;
    };
    enum TourKind : char { Picture = 'p', Statue = 's', GiftShop = 'g' };
    struct Tour {
        TourKind tourkind;
        unsigned int groupno;
    };

    TallyVotes( unsigned int voters, unsigned int group, Printer &printer );
    Tour vote( unsigned int id, Ballot ballot );
    void done(
#if defined( TASK ) // task solution
        unsigned int id
#endif
    );
  private:                                       // more private function
    void addBallot( TallyVotes::Ballot ballot ); // used to add ballot to vote result
    TallyVotes::TourKind getVoteResult();        // used to retrieve vote result
    void cleanUp();                              // cleanup the vote after a tour is completed
    void checkFailure();
#if defined( TASK ) // task solution
    Ballot currentBallot;
#endif
};


_Task Voter {
    unsigned int id;         // id of the voter
    unsigned int nvotes;     // how many times he has to go on tour
    TallyVotes &voteTallier; // the machine that hurt my brain
    Printer &printer;        // DA PRINTER

    void main();
    TallyVotes::Ballot cast() { // cast 3-way vote
        // O(1) random selection of 3 items without replacement using divide and
        // conquer.
        static const unsigned int voting[3][2][2] = { { { 2, 1 }, { 1, 2 } }, { { 0, 2 }, { 2, 0 } }, { { 0, 1 }, { 1, 0 } } };
        unsigned int picture = mprng( 2 ), statue = mprng( 1 );
        return ( TallyVotes::Ballot ){ picture, voting[picture][statue][0], voting[picture][statue][1] };
    }

  public:
    enum States : char {
        Start = 'S',
        Vote = 'V',
        Block = 'B',
        Unblock = 'U',
        Barging = 'b',
        Done = 'D',
        Complete = 'C',
        Going = 'G',
        Failed = 'X',
        Terminated = 'T'
    };
    Voter( unsigned int id, unsigned int nvotes, TallyVotes &voteTallier, Printer &printer );
};

_Monitor Printer { // chose one of the two kinds of type constructor
  private:
    struct VoterInfo {
        Voter::States voter_state;     // voter_state
        bool still_cold;               // used to control if any update is made
        TallyVotes::Ballot votes;      // the vote input
        unsigned int numBlocked_count; // number of people blked
        unsigned int group_number;     // group #
        TallyVotes::TourKind type;     // the type of tour he is going
    };

    VoterInfo *voter_information; // keep track of previous state
    unsigned int size;
    unsigned int tabCount;

    void renew();            // print all updated stuff, reset still_cold
    void reset_still_cold(); // used by renew to reset
  public:
    Printer( unsigned int voters );
    ~Printer(); // use to print the last line and thefinishing statement
    void print( unsigned int id, Voter::States state );
    void print( unsigned int id, Voter::States state, TallyVotes::Tour tour );
    void print( unsigned int id, Voter::States state, TallyVotes::Ballot vote );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked );
    void print( unsigned int id, Voter::States state, unsigned int numBlocked, unsigned int group );
};

#endif