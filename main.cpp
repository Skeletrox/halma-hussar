#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sys/stat.h>
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
	fstream playDataFile;
	inputFile.open("./input3.txt");
	StateVector initState{};
	string executionType = "SINGLE", s = "";
	char team = 'B';
	float timeLeft = 100.0;
	int counter = 0;
	while (inputFile >> s) {
		switch (counter) {
		case 0:
			// First line: Type of execution. By default is SINGLE
			if (s.length() > 0) {
				executionType = s;
			}
			break;
		case 1:
			// Second line: Team
			team = s == "WHITE" ? 'W' : 'B';
			break;
		case 2:
			// Third line: Number of seconds remaining
			try {
				timeLeft = stof(s);
			}
			catch (...) {
				// If there is an error in the input then timeLeft is default
			}
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
		The playdata.txt contains the time you can take for a move, and maybe a sequence of steps.
	*/
	if (executionType == "GAME") {
		cout << "Game" << endl;
		struct stat playFile;
		cout << stat("./playdata.txt", &playFile) << endl;
		if (stat("./playdata.txt", &playFile) != -1) {
			// File exists, read it.
			playDataFile.open("./playdata.txt");
			string remainingTime;
			playDataFile >> remainingTime;
			timeLeft = stof(remainingTime);
			cout << "Loaded time left: " << timeLeft << endl;
		}
		else {
			timeLeft = timeLeft / 45;
		}
		// The minimum game length is 45 moves, apparently.
	}
	State* currState = new State(initState, { {} }, NULL, true);
	currState->computeScore(team, board.getBase(team));
	int depth = getDepth(timeLeft, performanceMeasure, currState->getScore());
	PositionsVector playerPositions = getPositions(initState, team);
	cout << "Player positions are: " << endl;
	printPositions(playerPositions);
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
	for (State *x : currState->getChildren()) {
		cout << "State value is: " << x->getAlphaBetaPrediction() << endl;
		cout << "State move is: ";
		printPositions(x->getPositions());
	}
	State* desiredChild = currState->getDesiredChild();
	cout << endl << "desired child has result " << desiredChild->getScore() << ", address " << desiredChild << " and is at index " << currState->getDesiredChildLoc() << endl;
	cout << "Alpha Beta value for desired child is " << desiredChild->getAlphaBetaPrediction() << endl;
	cout << "Root is at " << currState << endl;
	string result = generateString(desiredChild->getPositions(), desiredChild->isStateAJump());
	ofstream outFile;
	outFile.open("./output.txt");
	outFile << result;
	outFile.close();
	if (executionType == "GAME") {
		// Some persistent data we may be able to use
		// Store the actual playtime.
		playDataFile.open("./playdata.txt", fstream::out);
		char* timeLeftString = (char*)malloc(20 * sizeof(char));
		snprintf(timeLeftString, 20, "%.4f", timeLeft);
		playDataFile << timeLeftString;
		playDataFile.close();
	}
	auto end = chrono::high_resolution_clock::now();
	long actual = chrono::duration_cast<chrono::microseconds>(end - start).count();
	cout << "Actual duration: " << actual << endl;
	return actual;
}

int main() {
	float performanceMeasure = calibrate();
	runProgram(performanceMeasure);
}