#pragma once
#include "components.h"
#include "State.h"
#include <cfloat>

class Board {
private:
	StateVector state;
	PositionsVector blackBase, whiteBase;
	std::map<std::array<int, 2>, bool>* visited;

public:
	Board(StateVector inpState);
	PositionsVector getBase(char team);
	State* generateMinMaxTree(State *s, int turnCount, PositionsVector argLocations, float alpha, float beta, bool max);
};

