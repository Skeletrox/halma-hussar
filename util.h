#pragma once
#include "components.h"
#include "State.h"

PositionsVector getPositions(StateVector boardState, char team);

bool isJump(PositionsVector positions);

float utility(int x, int y);

State* doAlphaBetaPruning(State *root);

float doMinValue(State* state, float alpha, float beta);
float doMaxValue(State* state, float alpha, float beta);