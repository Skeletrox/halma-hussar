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
	State *currState = new State(initState, { {} }, NULL, true);
	int depth = getDepth(timeLeft, performanceMeasure);
	PositionsVector playerPositions = getPositions(initState, team);
	Player player = Player(team, playerPositions);
	int playerDepth = 3;
	/*
		Generate the minmax tree with the following attributes:
			The current State
			How deep can the player jump
			The number of turns
			The locations of the player's points
			Alpha and Beta [For Alpha-Beta Pruning]
	*/
	auto start = chrono::high_resolution_clock::now();
	currState = board.generateMinMaxTree(currState, playerDepth, depth, player.getLocations(), FLT_MIN, FLT_MAX, true);
	/*
	for (State c : currState.getChildren()) {
		cout << endl << c.getAlphaBetaPrediction() << "    " << c.getScore() << endl;
		cout << c.getChildren().size() << endl;
		for (State c2 : c.getChildren()) {
			cout << &c2<< "    " << c2.getAlphaBetaPrediction() << " and " << c2.getScore() <<  " " << endl;
			for (int i = 0; i < 16; i++) {
				for (int j = 0; j < 16; j++) {
					cout << c2.getState()[i][j];
				}
				cout << endl;
			}
			cout << endl << endl;
		}
	}
	*/
	State *desiredChild = currState->getDesiredChild();
	string result;
	PositionsVector move = desiredChild->getPositions();
	result.append(isJump(move) ? "J " : "E ");
	for (array<int, 2> m : move) {
		char *currstring = (char*) malloc(40);
		snprintf(currstring, 40, "%d,%d ", m[0], m[1]);
		result.append(currstring);
	}
	char* immutableResult = (char*)malloc(result.size());
	ofstream outFile;
	outFile.open("./output.txt");
	outFile << result;
	outFile.close();
	auto end = chrono::high_resolution_clock::now();
	long actual = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "Actual duration: " << actual;
	return actual;
}

int main() {
	float performanceMeasure = calibrate();
	runProgram(performanceMeasure);
}