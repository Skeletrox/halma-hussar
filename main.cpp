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
	for (array<int, 2> pos : playerPositions) {
		cout << pos[0] << " " << pos[1] << endl;
	}
	cout << "-------------------------------------" << endl;
	Player player = Player(team, playerPositions);
	int numTurns = 2, playerDepth = 4;
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
	for (State c : currState.getChildren()) {
		cout << endl << c.getAlphaBetaPrediction() << "    " << c.getScore() << endl;
		for (State c2 : c.getChildren()) {
			cout << "    " << c2.getAlphaBetaPrediction() << " and " << c2.getScore();
		}
	}
}