#pragma once
#include "components.h"

class Board {
private:
	StateVector state;
	PositionsVector blackBase, whiteBase;

public:
	Board(StateVector inpState);
	
	PositionsVector getBase(char team);
};

