#pragma once
#include <iostream>
#include "components.h"
/*
	Stores a board state with the following values:
		1. The state of the board
		2. The appropriate move made to get the current state
		3. Is this a jump?
		4. Score
*/
class State
{
private:
	StateVector state;
	PositionsVector positions;
	bool jump;
	State* parent;
	int desiredChildLoc;
	std::vector<State> children;
	float score, alphaBetaPrediction;

public:
	State(StateVector inpState, PositionsVector inPositions, State *inParent, bool root);
	// Getter and setter for children
	std::vector<State> getChildren();
	void setFutureStates(PositionsVector positions, int level, std::map<std::array<int, 2>, bool> visited);
	State getDesiredChild();
	void setDesiredChildLoc(int i);
	void setChildren(std::vector<State> argChildren);

	// Getter and setter for score and alphaBetaPrediction
	void setScore(char player, PositionsVector playersBases);
	float getScore();
	void setAlphaBetaPrediction(float value);
	float getAlphaBetaPrediction();

	// Getter for state
	StateVector getState();
	
};

