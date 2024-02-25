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
extern Mix_Chunk* player1Sound;
extern Mix_Chunk* player2Sound;

extern bool twoPlayerMode;

enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT
};

struct Snake {
	Snake(GameWorld* world, playerNum playerNum, std::string name);

	void render(SDL_Renderer* renderer);

	void move(Uint32 &accumulatedTime, GameWorld* gameWorld);
	void moveByOne(GameWorld* gameWorld);
	CollisionType checkCollisions(Snake* snakeToCheckAgainst);

	void increaseSize();

	void setSnakeColor(SDL_Color color);
	void setSnakePosition(SDL_Point point);
	void setBody(GameWorld* world);
	void setBody(GameWorld* world, playerNum player2);
	void setSpeed(uint sp);
	void setDirection(Direction dir);
	void setDesiredDirection(Direction dir);
	void increaseSpeed(uint inc);
	void setWallCollision(bool collidedWithWall);

	Cell* getHead();
	std::vector<Cell>* getBody();
	SDL_Color getColor();
	SDL_Point getHeadPosition();
	GridPosition getGridPosition(); // position in the grid of the head (row, col)
	Direction getDirection();
	std::string getName();
	bool getWallCollision();
	uint getSpeed();

private:
	// TODO: add an array of grid positions so that resizing the snake will be easier
	Cell head;
	std::vector<Cell> body;
	Texture* headTexture;
	SDL_Color bodyColor;

	std::string name;

	GridPosition gridPosition;
	SDL_Point headPosition;
	uint speed;
	uint bodyLength;
	Direction direction;
	Direction desiredDirection;
	bool wallCollision;
};

struct PickUp {
	PickUp();

	virtual void handle(Snake snek);
	virtual void render(SDL_Renderer* renderer);

	virtual Cell getCell();
	GridPosition getPosition();

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
	GridPosition getPosition();
};

#endif // !ACTOR_H
