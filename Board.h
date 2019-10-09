#pragma once
#include "components.h"

class Board {
private:
	StateVector state;
	PositionsVector blackBase, whiteBase;

public:
	Board(StateVector inpState);
	FutureStatesMap getFutureStates(StateVector state, PositionsVector positions);
};

