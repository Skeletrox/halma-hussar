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
				neededVector.push_back({ j, i });
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
	for (State *s : state->getChildren()) {
		v = max(v, doMinValue(s, alpha, beta));
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
	for (State *s : state->getChildren()) {
		v = min(v, doMaxValue(s, alpha, beta));
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

long calibrate() {
	auto start = std::chrono::high_resolution_clock::now();
	int x = 0;
	for (int i = 0; i < 1000; i++) {
		x++;
	}
	auto end = std::chrono::high_resolution_clock::now();
	long diff = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
	return diff;
}

int getDepth(float timeRemaining, long calibratedValue) {
	/*
		The hard time limits are as follows:
			The following time limits are generated on a calibration factor of 1,
			i.e. incrementing a variable 1000 times takes 1 second
			Precalculated values for execution time for number of turns is (with a max Jump of 3, i.e. the agent will only jump 3 times):
			depthMicroSeconds = {
				1: 25827,
				2: 197755,
				3: 6052148,
				4: 11055020,
				5: 275469824,
				6: 721433536
			}
			A way to maximize output is to perform thia again on the child node if more time can be spared.

			The time remaining can be expressed in microseconds, and divide by calibratedValue, as a slower computer
			can cause our values to be awry.
	*/
	float timeRemainingMicrosec = timeRemaining * 1000000 / calibratedValue;
	std::array<long, 6> times{ 25827, 197755, 6052148, 11055020, 275469824, 721433536 };

	// The returnable contains 2 integers, how deep to go, and how many times to go deep
	for (int i = 0; i < times.size()-1; i++) {
		if (timeRemainingMicrosec < times[i + 1]) {
			// We don't have enough time to deepen to the next level; stop here
			std::cout << "Expected duration: " << times[i] << std::endl;
			return i + 1; // Zero-indexed array
		}
	}
	return times.size();
}

bool isIllegal(int xStart, int yStart, int xEnd, int yEnd, PositionsVector baseAnchors, char team) {
	if (team == 'B') {
		// 1. Your own base is the baseAnchors. Make sure that your piece, if not in your base, does not jump back
		// 2. Your opponent's base is at 15 - baseAnchors. Do not jump out of it.
		if (!found(xStart, yStart, baseAnchors, false) && found(xEnd, yEnd, baseAnchors, false)) { // Case 1
			return true;
		} else if (found(xStart, yStart, baseAnchors, true) && !found(xEnd, yEnd, baseAnchors, true)) { // Case 2
			return true;
		}
	}
	else {
		// The above lines switched in context.
		if (!found(xStart, yStart, baseAnchors, true) && found(xEnd, yEnd, baseAnchors, true)) { // Case 1
			return true;
		} else if (found(xStart, yStart, baseAnchors, false) && !found(xEnd, yEnd, baseAnchors, false)) { // Case 2
			return true;
		}
	}
	// Legal move only
	return false;
}

bool found(int x, int y, PositionsVector baseAnchors, bool reverse) {
	for (std::array<int, 2> b : baseAnchors) {
		if ((x == (reverse ? 15 - b[0] : b[0])) && (y == (reverse ? 15 - b[1] : b[1]))) {
			return true;
		}
	}
	return false;
}
