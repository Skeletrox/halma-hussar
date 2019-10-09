/*
	Utility file. Define utility functions such as position initialization here.
*/
#include "util.h"
#include <math.h>


// Returns the positions of all the pieces of a certain team.
PositionsVector getPositions(StateVector boardState, char team) {
	PositionsVector neededVector;
	// Iterate through the 16x16 board
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (boardState[i][j] == team) {
				neededVector.push_back({ i, j });
			}
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
	return ( numerator / sqrt(x * x + y * y));
}