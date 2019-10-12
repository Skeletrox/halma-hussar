#include "Board.h"
#include "util.h"
#include <vector>
#include <array>
#include <iostream>



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
		std::array<int, 2> currWhite, currBlack;
		for (int j = 0; j < 2; j++) {
			currBlack[j] = baseAnchors[i][j];
			currWhite[j] = 15 - baseAnchors[i][j];
		}
		blackBase.push_back(currBlack);
		whiteBase.push_back(currWhite);
	}

}


PositionsVector Board::getBase(char team) {
	return (team == 'B' ? blackBase : whiteBase);
}


/*
	Take the first state s, generate its children and their children and so on until "depth" times
	If there is a jump, also consider the next "child" states [can snowball!!!]
	Handle snowballing by using visited.
	However, for each piece, there are upto 8, implying upto 152 moves per level, not counting jumps
	This implies that for a depth of 3 moves, you look at 7 million moves. Use alpha beta pruning?
*/
State* Board::generateMinMaxTree(State *parent, int jumpDepth, int turnCount, PositionsVector argLocations, float alpha, float beta, bool isMax) {
	
	std::map<std::array<int, 2>, bool> visited;
	visited.insert(std::pair<std::array<int, 2>, bool>({}, false));
	/*
		Get the opponent's locations how?
			Given your pieces, calculate the appropriate PositionsVector for the other player?
			Get the symbol in argLocations[0][0], then get the appropriate PositionVector of the other team?
			Also try to incorporate Alpha Beta Pruning here itself to avoid unnecessary expansion
	*/
	char team = parent->getState()[argLocations[0][1]][argLocations[0][0]];

	// Create all the child states of this parent
	parent->setFutureStates(argLocations, jumpDepth, visited, team, blackBase);

	char opponentTeam = team == 'B' ? 'W' : 'B';
	PositionsVector opponentPositions = getPositions(parent->getState(), opponentTeam);
	// If we have reached the depth then we shall return the utility of this board
	if (turnCount == 0) {
		/*
			If isMax is true, then the player is the one making the terminal move, else it's the other player
			We need to get the utility only for the terminal player
		*/
		parent->setScore(team, isMax ? argLocations : opponentPositions);
		parent->setAlphaBetaPrediction(parent->getScore());
		return parent;
	}
	// If the node is a MAX expander, set the value to -inf, else set it to inf
	float v = isMax ? FLT_MIN : FLT_MAX;

	// Get all the children
	std::vector<State *> children = parent->getChildren();
	for (int i = 0; i < children.size(); i++) {
		children[i] = generateMinMaxTree(children[i], jumpDepth, turnCount - 1, opponentPositions, alpha, beta, !isMax);
		float result = children[i]->getAlphaBetaPrediction();
		/*
				Some properties:
					If isMax is false, then this is a min expander
					Alpha is always less that Beta
					For a maxNode, v being larger than beta implies that the parent is NOT going to choose this
					For a minNode, v being less than alpha implies the same
		*/
		if ((isMax && result > v) || (!isMax && result < v)) {
				parent->setDesiredChildLoc(i);
				v = result;
		}
		if ((isMax && v >= beta) || (!isMax && result <= alpha)) {
				parent->setAlphaBetaPrediction(v);
				return parent;
		}
		if (isMax) {
			alpha = max(alpha, v);
		} else {
			beta = min(beta, v);
		}
	}
	parent->setChildren(children);
	parent->setAlphaBetaPrediction(v);
	return parent;
}