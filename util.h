#pragma once
#include "components.h"
#include "State.h"

PositionsVector getPositions(StateVector boardState, char team);

bool isJump(PositionsVector positions);

float utility(int x, int y);
float max(float a, float b);
float min(float a, float b);

State* doAlphaBetaPruning(State *root);

float doMinValue(State* state, float alpha, float beta);
float doMaxValue(State* state, float alpha, float beta);

void printState(State s);

float calibrate();