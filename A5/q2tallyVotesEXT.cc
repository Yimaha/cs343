
#include "q2tallyVotes.h"
#include <iostream>
using namespace std;

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer &printer )
    : voters( voters ), group( group ), numVotes( 0 ), doneVoter( 0 ), groupCounter( 0 ), numBlockedTour( 0 ), voteSum{ 0, 0, 0 }, printer( printer ) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ) {
    // naturally acquire mutual exclusion
    checkFailure();      // if not enough people we immediately bounce
    addBallot( ballot ); // add to ballot
    printer.print( id, Voter::States::Vote, ballot );
    if ( numVotes == group ) {
        // handle group done
        groupCounter += 1; // increase groupCounter
        printer.print( id, Voter::States::Complete, TallyVotes::Tour{ getVoteResult(), groupCounter } );
    } else {
        numBlockedTour += 1; // counter to number of things blocked during voting
        printer.print( id, Voter::States::Block, numBlockedTour );
        while ( voters - doneVoter >= group && numVotes < group ) {
            try {
                _Accept( vote, done );
            } catch ( uMutexFailure::RendezvousFailure & ) {
                break; // not enough people left
            }

            // check if it is woken due to done.
            // it is kinda inefficient, because only the first done that
            // supposively triggers quorum failure would matter
            // remaining would be caught by the try-catch instead.
            if ( numVotes < group ) {
                printer.print( id, Voter::States::Done );
            }
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
    // natural release lock
    return result;
}

void TallyVotes::done() {
    // barging detec;tion
    doneVoter += 1;
    // handling is done inside vote
}

void TallyVotes::checkFailure() {
    if ( voters - doneVoter < group ) {
        _Throw Failed();
    }
}
