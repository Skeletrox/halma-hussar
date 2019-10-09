#include "Board.h"
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
*/
State Board::generateMinMaxTree(State s, int depth, PositionsVector argLocations) {
	std::map<std::array<int, 2>, bool> visited;
	visited.insert(std::pair<std::array<int, 2>, bool>({}, false));
	s.setFutureStates(argLocations, depth, visited);
	return s;
}