// this files constains all functions that is shared amount 3 implementations

#include "q2tallyVotes.h"
#include <math.h>
using namespace std;

#ifdef NOOUTPUT
#define PRINT( stmt )
#else
#define PRINT( stmt ) stmt
#endif // NOOUTPUT

// contains all shared function for TallyVote

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