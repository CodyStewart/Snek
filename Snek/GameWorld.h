#pragma once
#ifndef GAMEWORLD_H
#define GAMEWORLD_H

#include "texture.h"

extern int CURRENT_SCREEN_WIDTH;
extern int UNIT_DISTANCE;

extern const int paddingY;

// Cells are always square, so length = width
struct Cell {
	Cell();

	void render(SDL_Renderer* renderer);

	void setColor(SDL_Color color);
	void setTexture(Texture* texture);
	void setDimensions(int dim);
	void setPosition(int x, int y);

	Texture* getTexture();
	SDL_Color getColor();
	SDL_Rect* getRect();
	int getWidth();
	int getLength();

private:
	Texture* cellTexture;
	SDL_Color cellColor;

	SDL_Rect cellBlock;

	int width;
	int length;
};

struct GameWorld {
	GameWorld(int unitDist, const int numRows, const int numCols);

	void render(SDL_Renderer* renderer);

	void resizeGameWorld(int newSize);

private:
	Cell grid[25][25];

	int unitDistance;
	int numOfRows;
	int numOfCols;
};

#endif // !GAMEWORLD_H
