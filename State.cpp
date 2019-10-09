#include "State.h"
#include "util.h"
#include <array>

State::State(StateVector inpState, PositionsVector inPositions, State *inParent) {
	state = inpState;
	positions = inPositions;
	jump = isJump(positions);
	parent = inParent;
}

std::array<float, 2> State::getScore() {
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
	// Get number of pieces of the player in his own base
	float piecesInBaseScore = 0.0;
	for (std::array<int, 2> loc : playersBases) {
		if (state[loc[1]][loc[0]] == player) {
			piecesInBaseScore++;
		}
	}
	// The intensity of the number of pieces in the base
	piecesInBaseScore /= 19;

	// Get the closeness of the points from the line y = x + c, where c is the appropriate constant
}