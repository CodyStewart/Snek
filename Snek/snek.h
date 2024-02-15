#pragma once
#ifndef SNEK_H
#define SNEK_H

#include <vector>

const int NUMOFCOLS = 25;
const int NUMOFROWS = NUMOFCOLS;

typedef int uint;

struct GridPosition {
	uint row;
	uint column;
};

enum playerNum {
	player1,
	player2
};

enum CollisionType {
	SELF,
	WALL,
	OTHER,
	NONE
};

#endif // !SNEK_H