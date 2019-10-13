#include "State.h"
#include "util.h"
#include <array>

using namespace std;

State::State(StateVector inpState, PositionsVector inPositions, State *inParent, bool root) {
	state = inpState;
	positions = inPositions;
	jump = !root && isJump(positions);
	parent = inParent;
	desiredChildLoc = -1;
}

float State::getScore() {
	// Gets the score for this state
	return score;
}

void State::computeScore(char player, PositionsVector playersBases) {
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
	// Prefer a jump over a non-jump
	score = jump ? totalScore + 2 : totalScore;
}

void State::setFutureStates(PositionsVector positions, int level, map<array<int, 2>, bool> *visited, char team, PositionsVector baseAnchors) {
	if (level == 0) {
		return;
	}
	std::vector<State*> allChildren{}, stepChildren, jumpChildren;
	std::pair<std::vector<State*>, int> stepResult, jumpResult;
	stepResult = getSteps(positions, team, baseAnchors);
	jumpResult = getJumps(positions, team, baseAnchors, visited);
	stepChildren = stepResult.first;
	jumpChildren = jumpResult.first;
	// Get the index of the best child from each result
	int stepChildrenIndex = stepResult.second, jumpChildrenIndex = jumpResult.second;

	if (stepChildrenIndex == -1 && jumpChildrenIndex == -1) {
		// No Children for this state
		return;
	}
	else if (stepChildrenIndex == -1) {
		// only Jump Children
		this->desiredChildLoc = jumpChildrenIndex;
	} else if (jumpChildrenIndex == -1) {
		// only Step Children
		this->desiredChildLoc = stepChildrenIndex;
	} else {
		// both exist
		if (stepChildren[stepChildrenIndex]->getScore() > jumpChildren[jumpChildrenIndex]->getScore()) {
			desiredChildLoc = stepChildrenIndex; // The better child
		}
		else {
			desiredChildLoc = stepChildren.size() + jumpChildrenIndex; // As jump results are appended AFTER step results 
		}
	}
	allChildren.insert(allChildren.end(), stepChildren.begin(), stepChildren.end());
	allChildren.insert(allChildren.end(), jumpChildren.begin(), jumpChildren.end());
	setChildren(allChildren);
}

std::pair<std::vector<State*>, int> State::getSteps(PositionsVector positions, char team, PositionsVector baseAnchors) {
	int numPositions = positions.size();
	/*
		Initialize best step index and the best step score to -1 and -infinity
	*/
	int bestStepIndex = -1;
	float bestStepScore = -FLT_MAX + 1;
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
				childState->computeScore(team, team == 'B' ? baseAnchors : getMirror(baseAnchors));
				if (childState->getScore() > bestStepScore) {
					bestStepIndex = stepChildren.size() - 1;
					bestStepScore = childState->getScore();
				}
				
			}
		}
	}
	return std::pair<std::vector<State*>, int>{stepChildren, bestStepIndex};
}

std::pair<std::vector<State*>, int> State::getJumps(PositionsVector positions, char team, PositionsVector baseAnchors, map<array<int, 2>, bool> *visited) {
	int numPositions = positions.size();

	/*
		Define the best jump index [which index gives the best jump] and set that as the desired child
		Define the best score to avoid having to access each state over and over again
	*/
	int bestJumpIndex = -1;
	float bestJumpScore = -FLT_MAX + 1;

	std::vector<State*> jumpChildren;
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

		for (int j = 0; j < 3; j++) {
			for (int k = 0; k < 3; k++) {
				int xNow = xAdjacents[j], yNow = yAdjacents[k];
				// Ignore the current cell and out of bounds
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
			if (newState[currY][currX] == '.') { // Do not jump over an empty cell
				continue;
			}
			int xFactor = int(x <= currX) - int(x == currX) - int(x > currX);
			int yFactor = int(y <= currY) - int(y == currY) - int(y > currY);
			// Check the next value, while ensuring that it is accessible
			int newTargetx = currX + xFactor, newTargety = currY + yFactor;
			// Ensure that the new target is reachable, hasn't been already visited
			if ((newTargetx > 15) || (newTargetx < 0) || (newTargety > 15) || (newTargety < 0) || visited->count({ newTargetx, newTargety }) > 0) {
				continue;
			}

			/*
				Move constraints:
					Do not jump back into your base, do not jump out of your opponent's base
			*/
			if (isIllegal(x, y, newTargetx, newTargety, baseAnchors, team)) {
				continue;
			}
			if (newState[newTargety][newTargetx] == '.') {
				// Perform the jump and update.
				newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
				newState[newTargety][newTargetx] = newState[y][x] ^ newState[newTargety][newTargetx];
				newState[y][x] = newState[y][x] ^ newState[newTargety][newTargetx];
				PositionsVector positionPair = { {x, y}, {newTargetx, newTargety} };
				visited->insert(std::pair<std::array<int, 2>, bool>({ newTargetx, newTargety }, true));
				State* childState = new State(newState, positionPair, this, false);
				/*
					There are two outcomes, either expand childState and potentially see a drop in utility
						Or just stop here and if this is the best you can get, save this.
				*/
				childState->computeScore(team, team == 'B' ? baseAnchors : getMirror(baseAnchors));

				/*
					Expand this child [perform another jump] and check its utility recursively.
				*/
				State* childStateWithChildren = new State(newState, positionPair, this, false);

				// Use this to get child states so that you can choose the appropriate child later
				// Only consider jumps from this new target
				std::pair<std::vector<State*>, int> result = childStateWithChildren->getJumps({ {newTargetx, newTargety} }, team, baseAnchors, visited);
				if (result.second != -1) {
					// We can squash a sequence of moves as a child exists for this child
					childStateWithChildren->setChildrenAndDesired(result.first, result.second);
					/*
								"Squash" this sequence of jumps into a single state.
								Also update the state member to reflect the state of the preferred child.
					*/
					State* childOfChild = childStateWithChildren->getDesiredChild();
					PositionsVector childOfChildPos = childOfChild->getPositions();
					PositionsVector childOfChildPosNew = PositionsVector(childOfChildPos.begin() + 1, childOfChildPos.end());
					PositionsVector squashedPositions = childStateWithChildren->getPositions();
					squashedPositions.insert(squashedPositions.end(), childOfChildPosNew.begin(), childOfChildPosNew.end());
					childStateWithChildren->setState(childOfChild->getState());
					childStateWithChildren->setPositions(squashedPositions);
					childStateWithChildren->computeScore(team, team == 'B' ? baseAnchors : getMirror(baseAnchors));
				} else {
					childStateWithChildren->setScore(childState->getScore());
				}
				float singleScore = childState->getScore(), expandedScore = childStateWithChildren->getScore();
				if (singleScore >= expandedScore) {
					jumpChildren.push_back(childState);
					if (singleScore > bestJumpScore) {
						bestJumpScore = singleScore;
						bestJumpIndex = jumpChildren.size() - 1; // The best jump was added last!
					}
				} else {
					jumpChildren.push_back(childStateWithChildren);
					if (expandedScore > bestJumpScore) {
						bestJumpScore = expandedScore;
						bestJumpIndex = jumpChildren.size() - 1;
						setDesiredChildLoc(bestJumpIndex);
						setScore(bestJumpScore);
					}
				}
			}
		}

	}
	return std::pair<std::vector<State*>, int>{jumpChildren, bestJumpIndex};;
}

StateVector State::getState() {
	return state;
}

std::vector<State *> State::getChildren() {
	return children;
}

State* State::getDesiredChild() {
	return children[desiredChildLoc];
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

void State::setScore(float newScore) {
	score = newScore;
}

void State::setState(StateVector s) {
	state = s;
}

void State::setPositions(PositionsVector p) {
	positions = p;
}

void State::setChildrenAndDesired(std::vector<State*> argChildren, int desiredLoc) {
	children = argChildren;
	desiredChildLoc = desiredLoc;
}