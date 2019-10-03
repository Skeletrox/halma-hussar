#include "Player.h"


Player::Player(char argTeam, PositionsVector argLocations) {
	team = argTeam;
	scoreArrange = 0;
	scoreCapture = 0;
	locations = argLocations;
}

char Player::getTeam() {
	return team;
}

float* Player::getScores() {
	float returnable[2] = { scoreArrange, scoreCapture };
	return returnable;
}

void Player::setScores(float* scores) {
	scoreArrange = scores[0];
	scoreCapture = scores[1];
}

int** Player::makeMove(char **state) {
	// Given a state, choose the appropriate movement and get a score
	// Get all future states for the current state
}

PositionsVector Player::getLocations() {
	return locations;
}

