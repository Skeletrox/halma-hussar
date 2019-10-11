#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "components.h"
#include "Board.h"
#include "Player.h"
#include "State.h"
#include "util.h"

using namespace std;

int main() {
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
	State currState = State(initState, { {} }, NULL, true);
	PositionsVector playerPositions = getPositions(initState, team);
	Player player = Player(team, playerPositions);
	int numTurns = 3, playerDepth = 4;
	/*
		Generate the minmax tree with the following attributes:
			The current State
			How deep can the player jump
			The number of turns
			The locations of the player's points
			Alpha and Beta [For Alpha-Beta Pruning]
	*/
	currState = board.generateMinMaxTree(currState, playerDepth, numTurns, player.getLocations(), FLT_MIN, FLT_MAX, true);
	cout << currState.getAlphaBetaPrediction() <<  "    " << currState.getScore() << endl;
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
	State desiredChild1 = currState.getDesiredChild();
	State desiredChild2 = desiredChild1.getDesiredChild();

	cout << "First move desired child number " << currState.getDesiredChildLoc() << endl;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			cout << desiredChild1.getState()[i][j];
		}
		cout << endl;
	}
	cout << endl << endl;

	cout << "Second move desired child number " << desiredChild1.getDesiredChildLoc() << endl;
	for (int i = 0; i < 16; i++) {
		for (int j = 0; j < 16; j++) {
			cout << desiredChild2.getState()[i][j];
		}
		cout << endl;
	}
	cout << endl << endl;

	cout << "Move is " << endl;
	PositionsVector move = desiredChild1.getPositions();
	if (isJump(move)) {
		cout << "J ";
	} else {
		cout << "E ";
	}
	for (array<int, 2> m : move) {
		cout << m[0] << "," << m[1] << " ";
	}
	cout << endl;
}