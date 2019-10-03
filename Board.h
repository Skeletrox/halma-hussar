#pragma once
#include <vector>
#include "components.h"

using namespace std;

class Board {
private:
	char** state;
	PositionsVector blackBase, whiteBase;

public:
	Board(char **inpState);
	FutureStatesMap getFutureStates(StateArray state, PositionsVector positions);
};

