#pragma once
#include <vector>
#include <array>
#include <map>
#define PositionsVector std::vector<std::array<int, 2>>
#define StateVector std::vector<std::vector<char>>
// #define FutureStatesMap std::map<PositionsVector, std::array<std::array<char, 16>, 16>>
#define FutureStatesMap std::map<PositionsVector, StateVector>

/*
	Additional data for reference:
		1. Vectors support indexing
		2. Maps support vectors as keys and values

*/