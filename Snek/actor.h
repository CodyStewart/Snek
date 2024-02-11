#pragma once
#ifndef ACTOR_H
#define ACTOR_H

#include <vector>

#include "snek.h"
#include "GameWorld.h"
#include "texture.h"

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct Snake {
	Snake(GameWorld* world);

	void render(SDL_Renderer* renderer);

	void move(Uint32 deltaT);
	void moveByOne();

	void increaseSize();

	void setSnakeColor(SDL_Color color);
	void setSnakePosition(SDL_Point point);
	void setBody(GameWorld* world);
	void setSpeed(uint sp);
	void setDirection(Direction dir);
	void setDesiredDirection(Direction dir);

	SDL_Point getHeadPosition();
	GridPosition getGridPosition(); // position in the grid of the head (row, col)
	Direction getDirection();

private:
	// TODO: add an array of grid positions so that resizing the snake will be easier
	Cell head;
	std::vector<Cell> body;
	Texture* headTexture;
	SDL_Color bodyColor;

	GridPosition gridPosition;
	SDL_Point headPosition;
	uint speed;
	uint bodyLength;
	Direction direction;
	Direction desiredDirection;
};

#endif // !ACTOR_H
