#include "State.h"
#include "util.h"
#include <array>

State::State(StateVector inpState, PositionsVector inPositions, State *inParent) {
	state = inpState;
	positions = inPositions;
	jump = isJump(positions);
	parent = inParent;
}

float State::getScore() {
	// Gets the score for this state
	return score;
}

void State::setScore(char player, std::map<char, PositionsVector> bases) {
	// Sets the score for this state
	/*
		Given a set of bases, the state returns the appropriate score for this player
			Reward for: Pieces in opponent's base, pieces towards opponent's base
			Punish for: Pieces in own base, pieces going everywhere except towards opponent's base

	*/
	PositionsVector playersBases = bases.at(player);
	// Get number of pieces of the player in his own base and the number of pieces in the opponent's base
	float piecesInBase = 0.0, piecesInOpponent = 0.0;
	for (std::array<int, 2> loc : playersBases) {
		if (state[loc[1]][loc[0]] == player) {
			piecesInBase++;
		} else if (state[15 - loc[1]][15 - loc[0]] == player) {
			piecesInOpponent++;
		}
	}
	// The intensity of the number of pieces in the base
	piecesInBase /= 19;
	piecesInOpponent /= 19;

	/* 
		Get the closeness of the points from their "target" destinations, i.e. their appropriate goals
		The most efficient result happens when the pieces move in like a phalanx
		Get the relativve superimposition of pieces with the region y = x + 4 and y = x - 4.
		If all pieces are in this region then the score is 1, else reduce using distance of the point from y = x
	*/

	// Only perform the above step for 19 pieces
	int pieceCounter = 0;
	float directionalScore = 0.0;
	bool breakable = false;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (state[i][j] == player) {
				// get the utility of this point [the distance from y = x]
				float score = utility(j, i);
				// Consider an anomaly if the point is outside y = x + 4 or y = x  - 4
				// All points on y = x + 4 or y = x - 4 are at a distance of (4-0)/(sqrt(2)) from y = x
				if (score > 2.828427) {
					directionalScore += 1 / score;
				} else {
					directionalScore += 1;
				}
			}
			// We have examined all 19 pieces
			if (++pieceCounter == 19) {
				breakable = true;
				break;
			}
		}
		// No need to loop further
		if (breakable) {
			break;
		}
	}

	// Get the average score
	directionalScore /= 19;

	/*
		Bring the self base score, diversion score and opponent base score together
			The opponent base score and diversion score affect the score POSITIVELY
			The self base score affects the score NEGATIVELY
	*/
	float totalScore = piecesInOpponent + directionalScore - piecesInBase;
	score = totalScore;
}