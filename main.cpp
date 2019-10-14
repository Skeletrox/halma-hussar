#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "components.h"
#include "Board.h"
#include "Player.h"
#include "State.h"
#include "util.h"
#include <chrono>
#include <cfloat>

using namespace std;

long runProgram(float performanceMeasure) {
	/*Take inputs and store them in a vector

		Line Sequence:
			First line: SINGLE / GAME => Type of evaluation, or whatever
			Second line: BLACK or WHITE, what your team is
			Third line: A strictly positive float indicating time left
			16 lines: The board state, defined as:
				W: White Piece
				B: Black Piece
				.: Empty Cell
	*/
	ifstream inputFile;
	inputFile.open("./input.txt");
	StateVector initState;
	string executionType, s;
	char team = 'B';
	float timeLeft = 100.0;
	int counter = 0;
	while (inputFile >> s) {
		switch (counter) {
		case 0:
			// First line: Type of execution
			executionType = s;
			break;
		case 1:
			// Second line: Team
			team = s == "WHITE" ? 'W' : 'B';
			break;
		case 2:
			// Third line: Number of seconds remaining
			timeLeft = stof(s);
			break;
		default:
			// The board
			vector<char> row(s.begin(), s.end());
			initState.push_back(row);
		}
		counter++;
	}
	Board board = Board(initState);
	/*
		If the execution type is a game, then check for a playdata.txt
		The playdata.txt contains the time you can take for a move.
	*/
	if (executionType == "GAME") {
		
		// The minimum game length is 45 moves, apparently.
		timeLeft = timeLeft / 45;
	}
	State *currState = new State(initState, { {} }, NULL, true);
	int depth = getDepth(timeLeft, performanceMeasure);
	PositionsVector playerPositions = getPositions(initState, team);
	Player player = Player(team, playerPositions);
	/*
		Generate the minmax tree with the following attributes:
			The current State
			How deep can the player jump
			The number of turns
			The locations of the player's points
			Alpha and Beta [For Alpha-Beta Pruning]
	*/
	auto start = chrono::high_resolution_clock::now();
	currState = board.generateMinMaxTree(currState, depth, player.getLocations(), -FLT_MAX + 1, FLT_MAX, true);
	/*
	for (State* s : currState->getChildren()) {
		cout << s->getScore() << endl;
		printPositions(s->getPositions());
	}*/
	State *desiredChild = currState->getDesiredChild();
	cout << endl << "desired child has result " << desiredChild->getScore() << ", address " << desiredChild << " and is at index " << currState->getDesiredChildLoc() << endl;
	cout << "Alpha Beta value for desired child is " << desiredChild->getAlphaBetaPrediction() << endl;
	cout << "Root is at " << currState << endl;
	string result = generateString(desiredChild->getPositions(), desiredChild->isStateAJump());
	ofstream outFile;
	outFile.open("./output.txt");
	outFile << result;
	outFile.close();
	auto end = chrono::high_resolution_clock::now();
	long actual = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "Actual duration: " << actual << endl;
	PositionsVector dummy = { {0, 1}, {2, 3}, {4, 5} };
	return actual;
}

int main() {
	float performanceMeasure = calibrate();
	runProgram(performanceMeasure);
}