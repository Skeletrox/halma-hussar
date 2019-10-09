#include "State.h"
#include "util.h"
#include <array>

using namespace std;

State::State(StateVector inpState, PositionsVector inPositions, State *inParent, bool root) {
	state = inpState;
	positions = inPositions;
	jump = !root && isJump(positions);
	parent = inParent;
}

float State::getScore() {
	// Gets the score for this state
	return score;
}

void State::setScore(char player, PositionsVector playersBases) {
	// Sets the score for this state
	/*
		Given a set of bases, the state returns the appropriate score for this player
			Reward for: Pieces in opponent's base, pieces towards opponent's base
			Punish for: Pieces in own base, pieces going everywhere except towards opponent's base

	*/
	
	// Get number of pieces of the player in his own base and the number of pieces in the opponent's base
	float piecesInBase = 0.0, piecesInOpponent = 0.0;
	for (std::array<int, 2> loc : playersBases) {
		if (state[loc[1]][loc[0]] == player) {
			piecesInBase++;
		} else if (state[15 - loc[1]][15 - loc[0]] == player) {
			piecesInOpponent++;
		}
	}
	// The intensity of the number of pieces in the base
	piecesInBase /= 19;
	piecesInOpponent /= 19;

	/* 
		Get the closeness of the points from their "target" destinations, i.e. their appropriate goals
		The most efficient result happens when the pieces move in like a phalanx
		Get the relativve superimposition of pieces with the region y = x + 4 and y = x - 4.
		If all pieces are in this region then the score is 1, else reduce using distance of the point from y = x
	*/

	// Only perform the above step for 19 pieces
	int pieceCounter = 0;
	float directionalScore = 0.0;
	bool breakable = false;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			if (state[i][j] == player) {
				// get the utility of this point [the distance from y = x]
				float score = utility(j, i);
				// Consider an anomaly if the point is outside y = x + 4 or y = x  - 4
				// All points on y = x + 4 or y = x - 4 are at a distance of (4-0)/(sqrt(2)) from y = x
				if (score > 2.828427) {
					directionalScore += 1 / score;
				} else {
					directionalScore += 1;
				}
			}
			// We have examined all 19 pieces
			if (++pieceCounter == 19) {
				breakable = true;
				break;
			}
		}
		// No need to loop further
		if (breakable) {
			break;
		}
	}

	// Get the average score
	directionalScore /= 19;

	/*
		Bring the self base score, diversion score and opponent base score together
			The opponent base score and diversion score affect the score POSITIVELY
			The self base score affects the score NEGATIVELY
	*/
	float totalScore = piecesInOpponent + directionalScore - piecesInBase;
	score = totalScore;
}

void State::setFutureStates(PositionsVector positions, int level, map<array<int, 2>, bool> visited) {
	if (level == 0) {
		return;
	}
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
				PositionsVector positionPair = { {x, y}, {currX, currY} };
				State childState = State(newState, positionPair, this, false);
				children.push_back(childState);
				visited.insert(std::pair<std::array<int, 2>, bool>({ currY, currX }, true));
			}
			else {
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
					newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
					newState[newTargety][newTargetx] = newState[y][x] ^ newState[newTargety][newTargetx];
					newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
					PositionsVector positionPair = { {x, y}, {newTargetx, newTargety} };
					State childState = State(newState, positionPair, this, false);
					children.push_back(childState);
					// Use this to get child states so that you can choose the appropriate child later
					// Only consider the children from this new target point
					childState.setFutureStates({ {newTargetx, newTargety} }, level - 1, visited);
				}
			}
		}
	}
}

StateVector State::getState() {
	return state;
}

std::vector<State> State::getChildren() {
	return children;
}