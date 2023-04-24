// this files constains all functions that is shared amount 3 implementations

#include "q3tallyVotes.h"
#include <math.h>
using namespace std;

#ifdef NOOUTPUT
#define PRINT( stmt )
#else
#define PRINT( stmt ) stmt
#endif // NOOUTPUT

Printer::Printer( unsigned int voters ) : voter_information{ new VoterInfo[voters] }, size( voters ) {
    for ( unsigned i = 0; i < voters; i++ ) {
        // note that most of parameter doesn't matter unless still_cold is false
        voter_information[i] = {
            Voter::States::Start,
            true,                         // if true means no update is made
            { 3, 3, 3 },                  // no vote
            0,                            // no block count
            0,                            // no group number
            TallyVotes::TourKind::Picture // doesn't matter
        };                                // default everything
    }
    // print out the legend
    for ( unsigned i = 0; i < voters; i++ ) {
        PRINT( cout << "V" << i );
        if ( i != voters - 1 ) {
            PRINT( cout << "\t" );
        }
    }
    PRINT( cout << endl );
    for ( unsigned i = 0; i < voters; i++ ) {
        PRINT( cout << "*******" );
        if ( i != voters - 1 ) {
            PRINT( cout << "\t" );
        }
    }
    PRINT( cout << endl );
}

Printer::~Printer() {
    renew();
    PRINT( cout << "*****************" << endl );
    PRINT( cout << "All tours started" << endl );
    delete[] voter_information;
}
// for state S, D, X, T
void Printer::print( unsigned int id, Voter::States state ) {
    if ( !voter_information[id].still_cold ) {
        renew();
    }
    voter_information[id].voter_state = state;
    voter_information[id].still_cold = false;
}
// for state C, G
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Tour tour ) {
    Printer::print( id, state );
    voter_information[id].type = tour.tourkind;
    voter_information[id].group_number = tour.groupno;
}
// for state V
void Printer::print( unsigned int id, Voter::States state, TallyVotes::Ballot vote ) {
    Printer::print( id, state );
    voter_information[id].votes = vote;
}
// for state B, U
void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked ) {
    Printer::print( id, state );
    voter_information[id].numBlocked_count = numBlocked;
}

// for state b
void Printer::print( unsigned int id, Voter::States state, unsigned int numBlocked, unsigned int group ) {
    Printer::print( id, state, numBlocked );
    voter_information[id].group_number = group;
}

void Printer::renew() {
 PRINT(
    tabCount = 0;
    for ( unsigned i = 0; i < size; i++ ) {
        VoterInfo current_voter = voter_information[i];
        // this is an input that update a state
        if ( !current_voter.still_cold ) {
            for ( ; tabCount > 0; tabCount-- ) {
                cout << "\t";
            }
            // note that I could use a swtich statement, but it could get really messy so I use if/else instead
            if ( current_voter.voter_state == Voter::States::Start ) {
                cout << "S";
            } else if ( current_voter.voter_state == Voter::States::Vote ) {
                cout << "V " << current_voter.votes.picture << "," << current_voter.votes.statue << ","
                     << current_voter.votes.giftshop;
            } else if ( current_voter.voter_state == Voter::States::Block ) {
                cout << "B " << current_voter.numBlocked_count;
            } else if ( current_voter.voter_state == Voter::States::Unblock ) {
                cout << "U " << current_voter.numBlocked_count;
            } else if ( current_voter.voter_state == Voter::States::Barging ) {
                cout << "b " << current_voter.numBlocked_count << " " << current_voter.group_number;
            } else if ( current_voter.voter_state == Voter::States::Done ) {
                cout << "D";
            } else if ( current_voter.voter_state == Voter::States::Complete ) {
                cout << "C " << static_cast<char>( current_voter.type );
            } else if ( current_voter.voter_state == Voter::States::Going ) {
                cout << "G " << static_cast<char>( current_voter.type ) << " " << current_voter.group_number;
            } else if ( current_voter.voter_state == Voter::States::Failed ) {
                cout << "X";
            } else {
                cout << "T";
            }
        }
        tabCount += 1;
    }

    cout << endl;
 )
    reset_still_cold();
    // we print the row, and clean up the memory, leave the rest to the caller
}

void Printer::reset_still_cold() { // resetting everyone
    for ( unsigned i = 0; i < size; i++ ) {
        voter_information[i].still_cold = true;
    }
}

Voter::Voter( unsigned int id, unsigned int nvotes, TallyVotes &voteTallier, Printer &printer )
    : id( id ), nvotes( nvotes ), voteTallier( voteTallier ), printer( printer ) {}

// a criticle section
void TallyVotes::addBallot( Ballot ballot ) {
    voteSum[0] += ballot.picture;
    voteSum[1] += ballot.statue;
    voteSum[2] += ballot.giftshop;
    numVotes += 1;
}
// helper
TallyVotes::TourKind TallyVotes::getVoteResult() {
    if ( voteSum[2] >= voteSum[0] ) {
        if ( voteSum[2] >= voteSum[1] ) {
            return TallyVotes::TourKind::GiftShop;
        } else {
            return TallyVotes::TourKind::Statue;
        }
    } else {
        if ( voteSum[0] >= voteSum[1] ) {
            return TallyVotes::TourKind::Picture;
        } else {
            return TallyVotes::TourKind::Statue;
        }
    }
}

void TallyVotes::cleanUp() {
    voteSum[0] = 0; // reset vote counter
    voteSum[1] = 0; // reset vote counter
    voteSum[2] = 0; // reset vote counter
    numVotes = 0;   // reset voter counter
}

void Voter::main() {
    yield( mprng( 19 ) ); // yeild 0 - 19
    try {
        for ( unsigned int i = 0; i < nvotes; i++ ) {
            printer.print( id, Voter::States::Start );                // S
            yield( mprng( 4 ) );                                      // yield 0 - 4
            TallyVotes::Tour result = voteTallier.vote( id, cast() ); // toss in your vote
            yield( mprng( 4 ) );                                      // yield 0 - 4
            printer.print( id, Voter::States::Going, result );        // G
        }

    } catch ( TallyVotes::Failed & ) {
        printer.print( id, Voter::States::Failed ); // X
    }
    callDone(); // because barrier is a special case, we do this to avoid duplicated code
    printer.print( id, Voter::States::Terminated ); // F
}