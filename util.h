#pragma once
#include "components.h"

PositionsVector getPositions(StateVector boardState, char team);

bool isJump(PositionsVector positions);

float utility(int x, int y);