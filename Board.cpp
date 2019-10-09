#include "Board.h"
#include <vector>
#include <array>
#include <iostream>

using namespace std;

Board::Board(StateVector inpState) {
	state = inpState;
	/* Define the base anchors and build the bases as diagonally
		opposite mirrors.
		The black [top-left] bases are:
			0,0; 1,0; 2,0; 3,0; 4,0
			0,1; 1,1; 2,1; 3,1; 4,1
			0,2; 1,2; 2,2; 3,2
			0,3; 1,3; 2,3;
			0,4; 1,4

		The white [bottom-right] bases are:

								 14,11; 15,11;
						  13,12; 14,12; 15,12;
				   12,13; 13,13; 14,13; 15,13;
			11,14; 12,14; 13,14; 14,14; 15,14;
			11,15; 12,15; 13,15; 14,15; 15,15;
			       

				   

		Subtractng each of the top from (15,15) returns the white base coordinates.
	*/
	int baseAnchors[19][2] = {
		{0,0},
		{1,0},
		{2,0},
		{3,0},
		{4,0},
		{0,1},
		{1,1},
		{2,1},
		{3,1},
		{4,1},
		{0,2},
		{1,2},
		{2,2},
		{3,2},
		{0,3},
		{1,3},
		{2,3},
		{0,4},
		{1,4}
	};
	for (int i = 0; i < 19; i++) {
		array<int, 2> currWhite, currBlack;
		for (int j = 0; j < 2; j++) {
			currBlack[j] = baseAnchors[i][j];
			currWhite[j] = 15 - baseAnchors[i][j];
		}
		blackBase.push_back(currBlack);
		whiteBase.push_back(currWhite);
	}

}

//TODO: Update function sigature to consider depth
//TODO: Update function signature/working to avoid a piece in enemy base jumping out of the opponent's base
FutureStatesMap Board::getFutureStates(StateVector state, PositionsVector positions) {

	// How do you go about this?
	// For every move-able coin, you make states with it moved in the directions it can
	
	// Generate the next states for piece that can move one step
	// Choose from positions supplied
	FutureStatesMap futures;
	int numPositions = positions.size();
	for (int i = 0; i < numPositions; i++) {
		// The point is expressed as x, y
		// See if all adjacent points are okay
		array<int, 2> current = positions[i];
		int x = current[0];
		int y = current[1];
		// Get adjacent positions
		int xAdjacents[3] = { x - 1, x, x + 1 };
		int yAdjacents[3] = { y - 1, y, y + 1 };
		array<array<int, 2>, 8> adjacentCells;
		int counter = 0;
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				// Ignore the current cell
				if (yAdjacents[k] == y && xAdjacents[j] == x) {
					continue;
				}
				// Create a new array for the new points and append that to the adjacent cells
				array<int, 2> currentAdjacent = { xAdjacents[j], yAdjacents[k] };
				adjacentCells[counter++] = currentAdjacent;
			}
		}
		// Iterate through the list of all adjacent cells and ensure we return only the acceptable states.
		for (int j = 0; j < 8; j++) {
			StateVector newState(state);
			int currX = adjacentCells[j][0], currY = adjacentCells[j][1];
			if (currX < 0 || currX > 15 || currY < 0 || currY > 15) {
				continue;
			}
			// Generate the state with the current coin swapped with the void in the other cell
			// If there is something in that cell, check if it can be jumped over
			if (newState[currY][currX] == '.') {
				// This is an empty cell. Swap the position with it and append to the FutureStates vector.

				//XOR swapping
				newState[y][x] = newState[y][x] ^ newState[currY][currX];
				newState[currY][currX] = newState[y][x] ^ newState[currY][currX];
				newState[y][x] = newState[y][x] ^ newState[currY][currX];
				futures.insert(pair<PositionsVector, StateVector>({ {x, y}, {currX, currY} }, newState));
			} else {
				/* 
					Extend our search
					Convert a boolean into a step function
					If x < currX, to get to the adjacent cell, add 1 to it.
					If x == currX, then don't modify the x value.
					If x > currX, to get to the adjacent cell, subtract 1 from it.
					Do the same for y.
				*/
				int xFactor = int(x <= currX) - int(x == currX) - int(x > currX);
				int yFactor = int(y <= currY) - int(y == currY) - int(y > currY);
				// Check the next value, while ensuring that it is accessible
				int newTargetx = currX + xFactor, newTargety = currY + yFactor;
				if (newTargetx < 0 || newTargetx > 15 || newTargety < 0 || newTargety > 15) {
					continue;
				}
				if (newState[newTargety][newTargetx] == '.') {
					// Perform the jump and update.
					// recursion will be handled by the game, board need not worry.
					newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
					newState[newTargety][newTargetx] = newState[y][x] ^ newState[newTargety][newTargetx];
					newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
					futures.insert(pair<PositionsVector, StateVector>({ {x, y}, {newTargetx, newTargety}}, newState));
				}
			}
		}
	}
	return futures;
}