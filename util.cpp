/*
	Utility file. Define utility functions such as position initialization here.
*/
#include "util.h"
#include <math.h>
#include <chrono>

float max(float a, float b) {
	return a > b ? a : b;
}

float min(float a, float b) {
	return a < b ? a : b;
}


// Returns the positions of all the pieces of a certain team.
PositionsVector getPositions(StateVector boardState, char team) {
	PositionsVector neededVector;
	// Iterate through the 16x16 board
	int count = 0;
	bool breakable = false;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (boardState[i][j] == team) {
				neededVector.push_back({ i, j });
				count++;
				if (count == 19) {
					breakable = true;
					break;
				}
			}
		}
		if (breakable) {
			break;
		}
	}
	return neededVector;
}

// Returns true if the move involves a jump, useful for checking if recursion needs to be handled.
bool isJump(PositionsVector positions) {
	return (abs(positions[0][0] - positions[1][0]) > 1 || abs(positions[0][1] - positions[1][1]) > 1);
}

// Utility function, defined as the distance from (x, y) to y = x
float utility(int x, int y) {
	float numerator = float(x) - float(y);
	return ( numerator / float(sqrt(x * x + y * y)));
}


State* doAlphaBetaPruning(State* root) {
	if (root->getChildren().size() != 0) {
		root->setAlphaBetaPrediction(root->getScore());
	}
	return root;
}

float doMaxValue(State* state, float alpha, float beta) {
	if (state->getChildren().size() == 0) {
		return state->getScore();
	}
	float v = FLT_MIN;
	for (State s : state->getChildren()) {
		v = max(v, doMinValue(&s, alpha, beta));
		if (v >= beta){
			return v;
		}
		alpha = max(alpha, v);
	}
	return v;
}

float doMinValue(State* state, float alpha, float beta) {
	if (state->getChildren().size() == 0) {
		return state->getScore();
	}
	float v = FLT_MAX;
	for (State s : state->getChildren()) {
		v = min(v, doMaxValue(&s, alpha, beta));
		if (v <= alpha) {
			return v;
		}
		beta = min(beta, v);
	}
	return v;
}

void printState(State s) {
	std::vector<std::vector<char>> state = s.getState();
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			std::cout << state[i][j];
		}
		std::cout << std::endl;
	}
}

float calibrate() {
	auto start = std::chrono::high_resolution_clock::now();
	for (int i = 0; i < 1000; i++) {
		int x = 0;
	}
	auto end = std::chrono::high_resolution_clock::now();
	return std:: chrono::duration_cast<std::chrono::microseconds>(end - start).count();
}