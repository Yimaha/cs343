
#include "q2tallyVotes.h"
#include <iostream>
using namespace std;

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer &printer )
    : voters( voters ), group( group ), numVotes( 0 ), doneVoter( 0 ), groupCounter( 0 ), numBlockedTour( 0 ), voteSum{ 0, 0, 0 },
      printer( printer ) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id __attribute__( ( unused ) ), Ballot ballot ) {
    // naturally acquire mutual exclusion
    checkFailure();      // if not enough people we immediately bounce
    addBallot( ballot ); // add to ballot
    printer.print( id, Voter::States::Vote, ballot );
    if ( numVotes == group ) {
        // handle group done
        groupCounter += 1; // increase groupCounter
        printer.print( id, Voter::States::Complete, TallyVotes::Tour{ getVoteResult(), groupCounter } );
    } else {
        // we wait until either we are allowed to wakeup, or quorum failurei is detected
        WAITUNTIL( ( numVotes == group || voters - doneVoter < group ), block(id), unblock(id) );
    }
 
    TallyVotes::Tour result = { getVoteResult(), groupCounter }; // store the Tour result that will be returned
    bool successful_tour = numVotes == group;
    if ( numBlockedTour == 0 ) { // last guy need to clean up the party
        cleanUp();
    }
    // don't need to decide who to wake next, monitor OP
    if ( !successful_tour ) {
        checkFailure();
    }

    // natural release lock
    EXIT(); 
    return result;
}

void TallyVotes::done() {
    // barging detection
    doneVoter += 1;
    EXIT();
    // handling is done inside vote
}

void TallyVotes::checkFailure() {
    if ( voters - doneVoter < group ) {
        _Throw Failed();
    }
}

void TallyVotes::block(unsigned int id) {
    numBlockedTour += 1; // counter to number of things blocked during voting
    printer.print( id, Voter::States::Block, numBlockedTour );
}

void TallyVotes::unblock(unsigned int id) {
    numBlockedTour -= 1;
    printer.print( id, Voter::States::Unblock, numBlockedTour );
}