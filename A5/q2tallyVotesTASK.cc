

#include "q2tallyVotes.h"
#include <iostream>
using namespace std;

TallyVotes::TallyVotes( unsigned int voters, unsigned int group, Printer &printer )
    : voters( voters ), group( group ), numVotes( 0 ), doneVoter( 0 ), groupCounter( 0 ), numBlockedTour( 0 ), voteSum{ 0, 0, 0 },
      printer( printer ) {}

TallyVotes::Tour TallyVotes::vote( unsigned int id __attribute__( ( unused ) ), Ballot ballot ) {
    /**
    There are a few assumption I made as to what should stay in vote:
    1. vote action, that is cast in the vote and get the result
    2. check for Failure, because if a Failure happen it has to terminate the vote (nothing should return)
       if I left it inside the main, I cannot stop it from returning normally since it already returned
    3. printing has to happen in a specific order, because assignment requires to print S, G in Voter
       the only way we can ensure unblock correctly, we need to print it in vote (before it returns)
    */
    checkFailure();      // if not enough people we immediately bounce
    addBallot( ballot ); // add to ballot
    currentId = id;      // let the main know which id it is dealing with
    currentBallot = ballot;
    bench.wait();
    numBlockedTour -= 1;
    printer.print( id, Voter::States::Unblock, numBlockedTour );
    currentId = id; // let the main know which id it is dealing with

    TallyVotes::Tour result = { getVoteResult(), groupCounter }; // store the Tour result that will be returned
    bool successful_tour = numVotes == group;

    if ( !successful_tour ) {
        checkFailure();
    }
    return result;
}

void TallyVotes::done( unsigned int id ) {
    // barging detec;tion
    doneVoter += 1;
    currentId = id;
    // handling is inside main
}

void TallyVotes::checkFailure() {
    if ( voters - doneVoter < group ) {
        _Throw Failed();
    }
}

void TallyVotes::main() {
    for ( ;; ) {
        _Accept( ~TallyVotes ) { break; }
        or _Accept( vote ) {
            numBlockedTour += 1; // counter to number of things blocked during voting
            printer.print( currentId, Voter::States::Vote, currentBallot );
            printer.print( currentId, Voter::States::Block, numBlockedTour );
            if ( numVotes == group ) {
                groupCounter += 1; // increase groupCounter
                printer.print( currentId, Voter::States::Complete, TallyVotes::Tour{ getVoteResult(), groupCounter } );
                while ( !bench.empty() ) {
                    bench.signalBlock(); // we now need to wake everyone up
                    doneVoting();
                }
            }
        }
        or _Accept( done ) {
            printer.print( currentId, Voter::States::Done );
            if ( voters - doneVoter < group ) { // quorem failure
                while ( !bench.empty() ) {
                    // printer.print( currentId, Voter::States::Done );
                    bench.signalBlock(); // it doesn't matter for the rest, since they always fail
                }
            }
        }
    }
}

void TallyVotes::doneVoting() {
    if ( numBlockedTour == 0 ) { // last guy need to clean up the party
        cleanUp();
    }
}