
#include "BargingCheckVote.h"
#include "q3tallyVotes.h"
#include <iostream>
using namespace std;
TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer &printer )
    : m( uSemaphore( 1 ) ), tourWait( uSemaphore( 0 ) ), voters( voters ), group( group ), numVotes( 0 ),
      doneVoter( 0 ), groupCounter( 0 ), numBlockedTour( 0 ), voteSum{ 0, 0, 0 },
      printer( printer ) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ) {
    m.P(); // acquire mutual exclusion
    VOTER_ENTER;
    checkFailure(); // if not enough people we immediately bounce
    addBallot( ballot ); // add to ballot
    printer.print( id, Voter::States::Vote, ballot );
    VOTE_DONE;
    if ( numVotes == group ) {
        GROUP_DONE
        // handle group done
        groupCounter += 1; // increase groupCounter
        printer.print( id, Voter::States::Complete, TallyVotes::Tour{ getVoteResult(), groupCounter } );
    } else {
        numBlockedTour += 1; // counter to number of things blocked during voting
        printer.print( id, Voter::States::Block, numBlockedTour );

        tourWait.P( m ); // put it on the blk list
        numBlockedTour -= 1;
        printer.print( id, Voter::States::Unblock, numBlockedTour );
    }

    // should only reach if group size is meet or out of tourLock
    TallyVotes::Tour result = { getVoteResult(), groupCounter }; // store the Tour result that will be returned
    bool successful_tour = numVotes == group;
    // now decide who to wake up next
    // order should be : tour -> future voter
    if ( !tourWait.empty() ) {
        tourWait.V(); // A wake up
        // A immediately take over
    } else {
        cleanUp();
        // no one is waiting to vote or tour, let them come freely
        m.V(); // release the lock
    }
    // we only check failure if amount of people going on tour is not enough
    if ( !successful_tour ) {
        checkFailure();
    }
    return result; // result don't have to be returned immediately
}

void TallyVotes::done() {
    m.P();
    doneVoter += 1;
    // whenever voter is not enough, and a full group is NOT formed, we wake people up
    // if a full group is formed we don't need to worry about anything
    if ( voters - doneVoter < group && numVotes < group && !tourWait.empty() ) {
        // we have to signal otherwise it would never wake up
        tourWait.V();
    } else {
        m.V();
    }
}

void TallyVotes::checkFailure() {
    if ( voters - doneVoter < group ) {
        m.V();
        _Throw Failed();
    }
}
