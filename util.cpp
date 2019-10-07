/*
	Utility file. Define utility functions such as position initialization here.
*/
#include "util.h"

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