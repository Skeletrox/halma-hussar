#pragma once
#include "components.h"
#include "State.h"

class Board {
private:
	StateVector state;
	PositionsVector blackBase, whiteBase;

public:
	Board(StateVector inpState);
	PositionsVector getBase(char team);
	State generateMinMaxTree(State s, int depth, PositionsVector argLocations);
};

