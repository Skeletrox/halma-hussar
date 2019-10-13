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
		Get the relativity of superimposition of pieces with the region y = x + 4 and y = x - 4.
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

void State::setFutureStates(PositionsVector positions, int level, map<array<int, 2>, bool> *visited, char team, PositionsVector baseAnchors) {
	if (level == 0) {
		return;
	}
	std::vector<State*> allChildren{}, stepChildren, jumpChildren;
	stepChildren = getSteps(positions, team, baseAnchors);
	jumpChildren = getJumps(positions, team, baseAnchors, visited);
	allChildren.insert(allChildren.end(), stepChildren.begin(), stepChildren.end());
	allChildren.insert(allChildren.end(), jumpChildren.begin(), jumpChildren.end());
	setChildren(allChildren);
}

std::vector<State*> State::getSteps(PositionsVector positions, char team, PositionsVector baseAnchors) {
	int numPositions = positions.size();
	std::vector<State*> stepChildren{};
	for (int i = 0; i < numPositions; i++) {
		// The point is expressed as x, y
		// See if all adjacent points are okay
		array<int, 2> current = positions[i];
		int x = current[0];
		int y = current[1];
		// Get adjacent positions
		int xAdjacents[3] = { x - 1, x, x + 1 };
		int yAdjacents[3] = { y - 1, y, y + 1 };

		// Initialize the adjacent cells vector
		vector<array<int, 2>> adjacentCells;

		// counter to handle only 8 points being chosen instead of 9
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				int xNow = xAdjacents[j], yNow = yAdjacents[k];
				// Ignore the current cell, out of bounds, and visited cells
				if ((xNow > 15) || (xNow < 0) || (yNow > 15) || (yNow < 0) || (yNow == y && xNow == x)) {
					continue;
				}
				// Create a new array for the new points and append that to the adjacent cells
				array<int, 2> currentAdjacent = { xNow, yNow };
				adjacentCells.push_back(currentAdjacent);
			}
		}
		// Iterate through the list of all adjacent cells and ensure we return only the acceptable states.
		for (int j = 0; j < adjacentCells.size(); j++) {
			StateVector newState(state);
			int currX = adjacentCells[j][0], currY = adjacentCells[j][1];

			/*
				Move constraints:
					Do not jump back into your base, do not jump out of your opponent's base
			*/
			if (isIllegal(x, y, currX, currY, baseAnchors, team)) {
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

				// Create a child state object
				PositionsVector positionPair = { {x, y}, {currX, currY} };
				State* childState = new State(newState, positionPair, this, false);
				stepChildren.push_back(childState);
			}
		}
	}
	return stepChildren;
}

std::vector<State*> State::getJumps(PositionsVector positions, char team, PositionsVector baseAnchors, map<array<int, 2>, bool> *visited) {
	int numPositions = positions.size();
	std::vector<State*> jumpChildren{};
	for (int i = 0; i < numPositions; i++) {
		// The point is expressed as x, y
		// See if all adjacent points are okay
		array<int, 2> current = positions[i];
		int x = current[0];
		int y = current[1];
		// Get adjacent positions
		int xAdjacents[3] = { x - 1, x, x + 1 };
		int yAdjacents[3] = { y - 1, y, y + 1 };

		// Initialize the adjacent cells vector
		vector<array<int, 2>> adjacentCells;

		// counter to handle only 8 points being chosen instead of 9
		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				int xNow = xAdjacents[j], yNow = yAdjacents[k];
				// Ignore the current cell, out of bounds, and visited cells
				if ((xNow > 15) || (xNow < 0) || (yNow > 15) || (yNow < 0) || (yNow == y && xNow == x)) {
					continue;
				}
				// Create a new array for the new points and append that to the adjacent cells
				array<int, 2> currentAdjacent = { xNow, yNow };
				adjacentCells.push_back(currentAdjacent);
			}
		}
		for (int j = 0; j < adjacentCells.size(); j++) {
			StateVector newState(state);
			int currX = adjacentCells[j][0], currY = adjacentCells[j][1];
			int xFactor = int(x <= currX) - int(x == currX) - int(x > currX);
			int yFactor = int(y <= currY) - int(y == currY) - int(y > currY);
			// Check the next value, while ensuring that it is accessible
			int newTargetx = currX + xFactor, newTargety = currY + yFactor;
			if ((newTargetx > 15) || (newTargetx < 0) || (newTargety > 15) || (newTargety < 0) || visited->count({ newTargetx, newTargety }) > 0 || (newTargety == y && newTargetx == x)) {
				continue;
			}

			/*
				Move constraints:
					Do not jump back into your base, do not jump out of your opponent's base
			*/
			if (isIllegal(x, y, currX, currY, baseAnchors, team)) {
				continue;
			}
			if (newState[newTargety][newTargetx] == '.') {
				// Perform the jump and update.
				newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
				newState[newTargety][newTargetx] = newState[y][x] ^ newState[newTargety][newTargetx];
				newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
				PositionsVector positionPair = { {x, y}, {newTargetx, newTargety} };
				State* childState = new State(newState, positionPair, this, false);
				jumpChildren.push_back(childState);
				visited->insert(std::pair<std::array<int, 2>, bool>({ newTargetx, newTargety}, true));
				// Use this to get child states so that you can choose the appropriate child later
				// Only consider jumps from this new target
				childState->setChildren(childState->getJumps({ {newTargetx, newTargety} }, team, baseAnchors, visited));
			}
		}

	}
	return jumpChildren;
}

StateVector State::getState() {
	return state;
}

std::vector<State *> State::getChildren() {
	return children;
}

State* State::getDesiredChild() {
	return getChildren()[desiredChildLoc];
}

void State::setDesiredChildLoc(int i) {
	desiredChildLoc = i;
}

float State::getAlphaBetaPrediction() {
	return alphaBetaPrediction;
}

void State::setAlphaBetaPrediction(float value) {
	alphaBetaPrediction = value;
}

void State::setChildren(std::vector<State *> argChildren) {
	children = argChildren;
}

int State::getDesiredChildLoc() {
	return desiredChildLoc;
}

PositionsVector State::getPositions() {
	return positions;
}