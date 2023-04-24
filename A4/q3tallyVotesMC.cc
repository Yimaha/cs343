
#include "BargingCheckVote.h"
#include "q3tallyVotes.h"
#include <iostream>
using namespace std;
TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer &printer )
    : avail( true ), voters( voters ), group( group ), numVotes( 0 ), doneVoter( 0 ), groupCounter( 0 ),
      numBlockedVote( 0 ), numBlockedTour( 0 ), voteSum{ 0, 0, 0 }, printer( printer ) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id, Ballot ballot ) {
    m.acquire(); // acquire mutual exclusion
    VOTER_ENTER;
    checkFailure(); // if not enough people we immediately bounce
    if ( !avail ) {
        numBlockedVote += 1;
        printer.print( id, Voter::States::Barging, numBlockedVote, groupCounter );
        voteLock.wait( m ); // note we don't print anything for block before voting
        numBlockedVote -= 1;
    }

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
        if ( !voteLock.empty() ) { // prefer wake up people in voteLock first
            voteLock.signal();
        } else {
            avail = true;
        }
        tourLock.wait( m ); // put it on the blk list
        numBlockedTour -= 1;
        printer.print( id, Voter::States::Unblock, numBlockedTour );
    }

    // should only reach if group size is meet or out of tourLock

    TallyVotes::Tour result = { getVoteResult(), groupCounter }; // store the Tour result that will be returned
    bool successful_tour = numVotes == group;
    if ( tourLock.empty() ) { // last guy need to clean up the party
        cleanUp();
    }
    // now decide who to wake up next
    // order should be : tour -> waiting voter -> future voter
    if ( !tourLock.empty() ) {
        tourLock.signal();
        avail = false;
    } else if ( !voteLock.empty() ) {
        // we are done waking up all the tourists, now let the voter continue
        // note that we prefer to wake up people who is already waiting first
        voteLock.signal();
        avail = false;
    } else {
        // no one is waiting to vote or tour, let them come freely
        avail = true;
    }
    if ( !successful_tour ) {
        checkFailure();
    }
    m.release();   // release the lock
    return result; // result don't have to be returned immediately
}

void TallyVotes::done() {
    m.acquire();
    // barging detection
    if ( !avail ) {
        voteLock.wait( m );
    }

    doneVoter += 1;

    if ( voters - doneVoter < group && !tourLock.empty() ) {
        // this is the helper part, where we have less than enough people 
        // in the vote process to form any group, so we need to start waking up tour lock
        tourLock.signal();
        avail = false;
    } else if ( !voteLock.empty() ) {
        // notice that this task is actually taking a place that belongs to someone ready to vote
        // now you finish "barging" (well not really but you are very rude to take other people's place)
        // you need to give their spot back. either by doing this, or leave avail open
        voteLock.signal();
        avail = false;
    } else {
        avail = true;
    }
    m.release();
}
// helper to trigger the failure
void TallyVotes::checkFailure() {
    if ( voters - doneVoter < group ) {
        m.release();
        _Throw Failed();
    }
}