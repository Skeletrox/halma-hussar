#pragma once
#include <iostream>
#include "components.h"

/*
	Stores a board state with the following values:
		1. The state of the board
		2. The appropriate move made to get the positions
		3. Is this a jump?
		4. Score
*/
class State
{
private:
	StateVector state;
	PositionsVector positions;
	bool jump;
	State *parent;
	std::vector<State> children;
	float score;

public:
	State(StateVector inpState, PositionsVector inPositions, State *inParent);
	void setScore(char player, std::map<char, PositionsVector> bases);
	float getScore();
};

