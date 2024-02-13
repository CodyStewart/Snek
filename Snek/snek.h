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

#endif // !SNEK_H