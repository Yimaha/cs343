
#include "q2tallyVotes.h"
#include <iostream>
using namespace std;

// java simulation (holy shit java sucks)
void TallyVotes::wait() {
    bench.wait();               // wait until signalled
    while ( rand() % 2 == 0 ) { // multiple bargers allowed
        try {
            _Accept( vote, done ) { // accept barging callers
            }
            _Else { // do not wait if no callers
            }       // _Accept
        } catch ( uMutexFailure::RendezvousFailure & ) {
        }
    } // while
}

void TallyVotes::signalAll() { // also useful
    while ( !bench.empty() )
        bench.signal(); // drain the condition
}

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer &printer )
    : ticket( 0 ), serving( 0 ), voters( voters ), group( group ), numVotes( 0 ), doneVoter( 0 ), groupCounter( 0 ), numBlockedVote( 0 ),
      numBlockedTour( 0 ), voteSum{ 0, 0, 0 }, printer( printer ) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ) {
    // naturally acquire mutual exclusion
    checkFailure();               // if not enough people we immediately bounce
    while ( numVotes == group ) { // barging avoidance
        // if numVotes == group it means the previous group is still touring
        // thus we wait here until our term
        numBlockedVote += 1;
        printer.print( id, Voter::States::Barging, numBlockedVote, groupCounter );
        wait();
        numBlockedVote -= 1;
    }
    // bargin check
    VOTER_ENTER( group );

    addBallot( ballot ); // add to ballot
    printer.print( id, Voter::States::Vote, ballot );
    if ( numVotes == group ) {
        // handle group done
        groupCounter += 1; // increase groupCounter
        printer.print( id, Voter::States::Complete, TallyVotes::Tour{ getVoteResult(), groupCounter } );
    } else {
        numBlockedTour += 1; // counter to number of things blocked during voting
        printer.print( id, Voter::States::Block, numBlockedTour );
        unsigned int yourTicket = groupCounter + 1;
        while ( groupCounter != yourTicket && voters - doneVoter >= group ) {
            // you wait until it is your turn, or when quorum failure happens
            wait();
        }
        numBlockedTour -= 1;
        printer.print( id, Voter::States::Unblock, numBlockedTour );
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

    signalAll();
    // natural release lock
    VOTER_LEAVE( group );
    return result;
}

void TallyVotes::done() {
    // barging detection
    doneVoter += 1;
    if ( voters - doneVoter < group ) { // detected quorum failure
        signalAll();
    }
}

void TallyVotes::checkFailure() {
    if ( voters - doneVoter < group ) {
        _Throw Failed();
    }
}
