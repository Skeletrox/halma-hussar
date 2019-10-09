#include "Player.h"
#include <array>

Player::Player(char argTeam, PositionsVector argLocations) {
	team = argTeam;
	score = 0;
	locations = argLocations;
}

char Player::getTeam() {
	return team;
}

float Player::getScore() {
	return score;
}

int** Player::makeMove(StateVector state) {
	// Given a state, choose the appropriate movement and get a score
	// Get all future states for the current state
	return NULL;
}

PositionsVector Player::getLocations() {
	return locations;
}

