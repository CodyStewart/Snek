#pragma once
#ifndef ACTOR_H
#define ACTOR_H

#include <vector>
#include <SDL_mixer.h>

#include "snek.h"
#include "GameWorld.h"
#include "texture.h"

extern GameWorld gameWorld;
extern std::vector<PickUp*> pickupGathering;
extern uint gameScore;
extern Mix_Chunk* sound;

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct Snake {
	Snake(GameWorld* world);

	void render(SDL_Renderer* renderer);

	void move(Uint32 deltaT, GameWorld* gameWorld);
	void moveByOne(GameWorld* gameWorld);
	bool checkCollisions();

	void increaseSize();

	void setSnakeColor(SDL_Color color);
	void setSnakePosition(SDL_Point point);
	void setBody(GameWorld* world);
	void setSpeed(uint sp);
	void setDirection(Direction dir);
	void setDesiredDirection(Direction dir);
	void increaseSpeed(uint inc);

	std::vector<Cell>* getBody();
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

struct PickUp {
	PickUp();

	virtual void handle(Snake snek);
	virtual void render(SDL_Renderer* renderer);

	virtual Cell getCell();

protected:
	Cell puCell;
	GridPosition puPosition;
};

struct SizeUp : public PickUp {
	SizeUp();
	SizeUp(SDL_Color szColor, Cell* cell);

	void handle(Snake snek) override;
	void render(SDL_Renderer* renderer) override;
	Cell getCell() override;
};

#endif // !ACTOR_H
