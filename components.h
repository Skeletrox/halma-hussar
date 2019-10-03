#pragma once
#include <vector>
#include <array>
#include <map>
#define PositionsVector std::vector<std::array<int, 2>>
#define StateArray std::array<std::array<char, 16>, 16>
#define FutureStatesMap std::map<PositionsVector, std::array<std::array<char, 16>, 16>>