#pragma once
#include "components.h"
#include <array>

class Player {
private:
	// The team the player belongs to, either 'B' or 'W'
	char team;
	/*
		The score can be comprised of the following:
			1. The capture [How much of the opponent's base do you own?]
			2. How "ideal" is your board arrangement to allow you to capture the opponent's base?

		For step 2, we assume a line going between (0,0) and (15,15). Any move that ensures that the pieces stay
		as close as possible to the reference line is considered optimal. Step 2 must not overshadow step 1; it
		a tiebreaker in case of two equally well-off moves.
	*/
	float scoreCapture, scoreArrange;
	// The location of each of the player's pieces, defined by a nx2 matrix
	PositionsVector locations;

public:
	//Default constructor. Initializes the player's locations based on a given input.
	Player(char argTeam, PositionsVector argLocations);
	//Returns the team of the character
	char getTeam();
	// Getter for scores, returns an array of floats
	std::array<float,2> getScores();
	// Setter for scores, updates scores
	void setScores(float* scores);
	// Gets locations of all pieces of player
	PositionsVector getLocations();
	// Given a board state, chooses best possible move, returning a 2x2 matrix of [src, dest]
	int** makeMove(char** state);
	

};
