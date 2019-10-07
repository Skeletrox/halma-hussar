#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

int main() {
	cout << "Hello" << endl;
	// Take inputs and store them in a vector
	ifstream inputFile;
	inputFile.open("./input.txt");
	string s;
	vector<string> readInput;
	while (inputFile >> s) {
		readInput.push_back(s);
	}
	/*
		Line Sequence:
			First line: SINGLE / GAME => Type of evaluation, or whatever
			Second line: BLACK or WHITE, what your team is
			Third line: A strictly positive float indicating time left
			16 lines: The board state, defined as:
				W: White Piece
				B: Black Piece
				.: Empty Cell
	*/
	for (string s : readInput) {
		cout << s << endl;
	}
	/*
		Outputs: 
			E x_from,y_from x_to,y_to   Move to an empty cell
				OR
			J x_from,y_from x_to,y_to   Jump to an empty cell
			One move per line
	*/
}