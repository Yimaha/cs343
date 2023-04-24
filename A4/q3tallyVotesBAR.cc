
#include "BargingCheckVote.h"
#include "q3tallyVotes.h"
#include <iostream>
using namespace std;
TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer &printer )
    : uBarrier( group ), voters( voters ), group( group ), numVotes( 0 ), doneVoter( 0 ), groupCounter( 0 ),
      numBlockedTour( 0 ), voteSum{ 0, 0, 0 }, printer( printer ) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ) {
    // naturally have mutual exclusion
    checkFailure();
    addBallot( ballot ); // add to ballot
    printer.print( id, Voter::States::Vote, ballot );
    if ( numVotes == group ) {
        groupCounter += 1; // increase groupCounter
        printer.print( id, Voter::States::Complete, TallyVotes::Tour{ getVoteResult(), groupCounter } );
        uBarrier::block(); // should be the last call and wake everyone
    } else {
        numBlockedTour += 1; // counter to number of things blocked during voting
        printer.print( id, Voter::States::Block, numBlockedTour );
        uBarrier::block(); // put it on the blk list
        numBlockedTour -= 1;
        printer.print( id, Voter::States::Unblock, numBlockedTour );
    }

    if ( !successful_tour ) {
        checkFailure();
    }
    return result; // result don't have to be returned immediately
}

void TallyVotes::last() {
    result = { getVoteResult(), groupCounter };
    successful_tour = numVotes == group; // note the numVote only increase if someone actually voted
    cleanUp();                           // last to leave the party need to clean up
}

void TallyVotes::done( unsigned int id ) {
    doneVoter += 1;
    // whenever voter is not enough, and a full group is NOT formed, we wake people up
    // if a full group is formed we don't need to worry about anything
    if ( voters - doneVoter < group && waiters() != 0 ) {
        printer.print( id, Voter::States::Done );
        uBarrier::block(); // we help blocking but we are not going to increase the counter
    }
}

void TallyVotes::checkFailure() {
    if ( voters - doneVoter < group ) {
        _Throw Failed();
    }
}
