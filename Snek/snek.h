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

	//bool operator<(GridPosition rhs) {
	//	return this->column < rhs.column;
	//}

	bool operator==(GridPosition rhs) {
		return this->row == rhs.row && this->column == rhs.column;
	}
	//bool operator==(GridPosition& rhs) const {
	//	return this->row == rhs.row && this->column == rhs.column;
	//}
	//bool operator < (GridPosition rhs) {
	//	return ;
	//}
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