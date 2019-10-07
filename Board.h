#pragma once
#include <vector>
#include "components.h"

using namespace std;

class Board {
private:
	StateVector state;
	PositionsVector blackBase, whiteBase;

public:
	Board(StateVector inpState);
	FutureStatesMap getFutureStates(StateVector state, PositionsVector positions);
};

