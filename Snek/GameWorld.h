#pragma once
#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include <random>

#include "snek.h"
#include "texture.h"
#include "timer.h"

extern int CURRENT_SCREEN_WIDTH;
extern int UNIT_DISTANCE;
extern uint gameScore;

extern const int paddingY;

struct PickUp;
struct SizeUp;

extern std::vector<PickUp*> pickupGathering;

// Cells are always square, so length = width
struct Cell {
	Cell();

	void render(SDL_Renderer* renderer);

	void setColor(SDL_Color color);
	void setTexture(Texture* texture);
	void setDimensions(int dim);
	void setPosition(int x, int y);
	void setOccupied(bool isOccupied);
	void setGridPosition(int row, int col);

	Texture* getTexture();
	SDL_Color getColor();
	SDL_Rect* getRect();
	int getWidth();
	int getLength();
	bool getOccupied();
	GridPosition getGridPosition();

private:
	Texture* cellTexture;
	SDL_Color cellColor;

	SDL_Rect cellBlock;

	GridPosition cellGridPosition;

	int width;
	int length;
	bool occupied;
};

struct GameWorld {
	GameWorld(int unitDist, const int numRows, const int numCols);

	void render(SDL_Renderer* renderer);

	Uint32 generatePickups(Uint32 delta);

	void resizeGameWorld(int newSize);

	GridPosition convertCoordsToRowCol(int x, int y);

	Cell* getGrid();
	Cell* getCell(int row, int col);
	Uint32 getTimeSinceStartOfProgram();
	int getNumOfRows();
	int getNumOfCols();

	void reset();

private:
	Cell grid[NUMOFROWS][NUMOFCOLS];

	Timer timeSinceStartOfProgram;

	int unitDistance;
	int numOfRows;
	int numOfCols;
};

#endif // !GAMEWORLD_H
