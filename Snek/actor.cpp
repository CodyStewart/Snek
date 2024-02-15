#include "actor.h"

bool checkBodyCollisions(Snake snake) {
	std::vector<Cell>* snakeBody = snake.getBody();
	Cell snakeHead = (*snakeBody)[0];
	for (int secondCellCompared = 1; secondCellCompared < snakeBody->size(); secondCellCompared++) {
		if (snakeHead.getRect()->x == (*snakeBody)[secondCellCompared].getRect()->x && snakeHead.getRect()->y == (*snakeBody)[secondCellCompared].getRect()->y)
			return true;
	}

	return false;
}

bool checkWallCollisions(Snake* snake) {
	if (snake->getWallCollision()) {
		snake->setWallCollision(false);
		return true;
	}

	return false;
}

bool checkRivalCollisions(Snake* thisSnake, Snake* otherSnake) {
	std::vector<Cell>* otherSnakeBody = otherSnake->getBody();
	Cell thisSnakeHead = thisSnake->getBody()->front();

	for (int thisSnakeCell = 0; thisSnakeCell < otherSnakeBody->size(); thisSnakeCell++) {
		if (thisSnakeHead.getRect()->x == (*otherSnakeBody)[thisSnakeCell].getRect()->x && thisSnakeHead.getRect()->y == (*otherSnakeBody)[thisSnakeCell].getRect()->y)
			return true;
	}

	return false;
}

// check if the snake head is touching a pickup
void checkPickUpCollisions(Snake* snake) {
	std::vector<Cell>* snakeBody = snake->getBody();
	Cell snakeHead = (*snakeBody)[0];
	SDL_Rect* snakeHeadRect = snakeHead.getRect();

	for (std::vector<PickUp*>::iterator iter = pickupGathering.begin(); iter != pickupGathering.end(); iter++) {
		Cell pickupCell = (**iter).getCell();
		SDL_Rect* pickupRect = pickupCell.getRect();
		if (snakeHeadRect->x == pickupRect->x && snakeHeadRect->y == pickupRect->y) {
			snake->increaseSize();
			uint newSpeed = 10 + gameScore;
			snake->setSpeed((uint)(newSpeed / 2));
			pickupGathering.erase(iter);

			if (snake->getName() == "Player 1")
				Mix_PlayChannel(-1, player1Sound, 0);
			else if (snake->getName() == "Player 2")
				Mix_PlayChannel(-1, player2Sound, 0);
			else
				printf("Neither sound was played while collecting pickup. Have the player names been changed?");

			gameScore++;
			break;
		}
	}
}

Snake::Snake(GameWorld* world, playerNum playerNum, std::string snakeName) {
	if (playerNum == player1) {
		setBody(world);
		bodyColor = { 0, 0, 255 };
		name = snakeName;
		headTexture = nullptr;
		headPosition = getHeadPosition();
		speed = 5;
		bodyLength = (uint)body.size();
		direction = UP;
		desiredDirection = UP;
		wallCollision = false;
	}
	else {
		if (twoPlayerMode)
			setBody(world, player2);
		bodyColor = { 255,255,0 };
		name = snakeName;
		headTexture = nullptr;
		headPosition = getHeadPosition();
		speed = 5;
		bodyLength = (uint)body.size();
		direction = LEFT;
		desiredDirection = LEFT;
		wallCollision = false;
	}
}

void Snake::move(Uint32 &accumulatedTime, GameWorld* gameWorld) {
	// check if enough time has past for the snake to have moved
	Uint32 timeToMove = 1000 / speed;

	if (accumulatedTime >= timeToMove) {
		accumulatedTime = 0;
		moveByOne(gameWorld);
	}
}

void Snake::moveByOne(GameWorld* gameWorld) {
	// move the snake one space based on direction
	Cell tempBodyCell;

	switch (desiredDirection)
	{
		case UP:
			if (gridPosition.row > 0) {
				// unset the cell uncovered by moving the tail
				Cell tailCell = body[body.size() - 1];
				GridPosition gp = gameWorld->convertCoordsToRowCol(tailCell.getRect()->x, tailCell.getRect()->y);
				Cell* gridCellUncovered = gameWorld->getCell(gp.row, gp.column);
				gridCellUncovered->setOccupied(false);

				// work backwards from the tail and update the positions of the body
				for (size_t bodyIndex = body.size() - 1; bodyIndex > 0; bodyIndex--) {
					tempBodyCell = body[bodyIndex - 1];
					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head up one cell
				head.setPosition(head.getRect()->x, head.getRect()->y - head.getLength());
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position up one
				gridPosition.row -= 1;

				direction = UP;
			}
			else {
				wallCollision = true;
			}
			break;
		case DOWN:
			if (gridPosition.row < NUMOFROWS - 1) {
				// unset the cell uncovered by moving the tail
				Cell tailCell = body[body.size() - 1];
				GridPosition gp = gameWorld->convertCoordsToRowCol(tailCell.getRect()->x, tailCell.getRect()->y);
				Cell* gridCellUncovered = gameWorld->getCell(gp.row, gp.column);
				gridCellUncovered->setOccupied(false);

				// work backwards from the tail and update the positions of the body
				for (size_t bodyIndex = body.size() - 1; bodyIndex > 0; bodyIndex--) {
					tempBodyCell = body[bodyIndex - 1];
					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head down one cell
				head.setPosition(head.getRect()->x, head.getRect()->y + head.getLength());
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position down one
				gridPosition.row += 1;

				direction = DOWN;
			}
			else {
				wallCollision = true;
			}
			break;
		case LEFT:
			if (gridPosition.column > 0) {
				// unset the cell uncovered by moving the tail
				Cell tailCell = body[body.size() - 1];
				GridPosition gp = gameWorld->convertCoordsToRowCol(tailCell.getRect()->x, tailCell.getRect()->y);
				Cell* gridCellUncovered = gameWorld->getCell(gp.row, gp.column);
				gridCellUncovered->setOccupied(false);

				// work backwards from the tail and update the positions of the body
				for (size_t bodyIndex = body.size() - 1; bodyIndex > 0; bodyIndex--) {
					tempBodyCell = body[bodyIndex - 1];
					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head left one cell
				head.setPosition(head.getRect()->x - head.getWidth(), head.getRect()->y);
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position left one
				gridPosition.column -= 1;

				direction = LEFT;
			}
			else {
				wallCollision = true;
			}
			break;
		case RIGHT:
			if (gridPosition.column < NUMOFCOLS - 1) {
				// unset the cell uncovered by moving the tail
				Cell tailCell = body[body.size() - 1];
				GridPosition gp = gameWorld->convertCoordsToRowCol(tailCell.getRect()->x, tailCell.getRect()->y);
				Cell* gridCellUncovered = gameWorld->getCell(gp.row, gp.column);
				gridCellUncovered->setOccupied(false);

				// work backwards from the tail and update the positions of the body
				for (size_t bodyIndex = body.size() - 1; bodyIndex > 0; bodyIndex--) {
					tempBodyCell = body[bodyIndex - 1];
					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head right one cell
				head.setPosition(head.getRect()->x + head.getWidth(), head.getRect()->y);
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position right one
				gridPosition.column += 1;
				direction = RIGHT;
			}
			else {
				wallCollision = true;
			}
			break;
		default:
			break;
	}

	// set the cell now under the head to occupied
	Cell headCell = body[0];
	GridPosition gp2 = gameWorld->convertCoordsToRowCol(headCell.getRect()->x, headCell.getRect()->y);
	Cell* gridCellCovered = gameWorld->getCell(gp2.row, gp2.column);
	gridCellCovered->setOccupied(true);
}

CollisionType Snake::checkCollisions(Snake* snakeToCheckAgainst) {
	if (checkBodyCollisions(*this)) {
		return SELF;
	}
	else if (checkWallCollisions(this)) {
		return WALL;
	}
	else if (twoPlayerMode && checkRivalCollisions(this, snakeToCheckAgainst)) {
		return OTHER;
	}
	
	checkPickUpCollisions(this);

	return NONE;
}

void Snake::increaseSize() {
	Cell newCell = Cell();

	// figure out where the new cell needs to go
	Cell tailCell = body[body.size() - 1];
	Cell penultimateCell = body[body.size() - 2];

	newCell.setColor(tailCell.getColor());
	newCell.setDimensions(tailCell.getLength());

	SDL_Rect* tailCellRect = tailCell.getRect();
	SDL_Rect* penultCellRect = penultimateCell.getRect();

	// compare the x and y coordinates to figure out the "direction" the tail is going
	if (tailCellRect->x < penultCellRect->x) {
		// tail is to the left of penultimate cell, add new cell to the left of tail
		newCell.setPosition(tailCellRect->x - tailCellRect->w, tailCellRect->y);
		GridPosition gp = gameWorld.convertCoordsToRowCol(tailCellRect->x - tailCellRect->w, tailCellRect->y);
		gameWorld.getCell(gp.row, gp.column)->setOccupied(true);
	}
	else if (tailCellRect->x > penultCellRect->x) {
		// tail is to the right of penultimate cell, add new cell to the right of tail
		newCell.setPosition(tailCellRect->x + tailCellRect->w, tailCellRect->y);
		GridPosition gp = gameWorld.convertCoordsToRowCol(tailCellRect->x + tailCellRect->w, tailCellRect->y);
		gameWorld.getCell(gp.row, gp.column)->setOccupied(true);
	}
	else if (tailCellRect->y < penultCellRect->y) {
		// tail is above penultimate cell, add new cell above tail
		newCell.setPosition(tailCellRect->x, tailCellRect->y - tailCellRect->h);
		GridPosition gp = gameWorld.convertCoordsToRowCol(tailCellRect->x, tailCellRect->y - tailCellRect->h);
		gameWorld.getCell(gp.row, gp.column)->setOccupied(true);
	}
	else if (tailCellRect->y > penultCellRect->y) {
		// tail is below penultimate cell, add new cell below tail
		newCell.setPosition(tailCellRect->x, tailCellRect->y + tailCellRect->h);
		GridPosition gp = gameWorld.convertCoordsToRowCol(tailCellRect->x, tailCellRect->y + tailCellRect->h);
		gameWorld.getCell(gp.row, gp.column)->setOccupied(true);
	}
	else { // we should not be here
		printf("ERROR! increaseSize() function has entered a section of code it should not be in!\n");
	}

	newCell.setOccupied(true);
	body.push_back(newCell);
	bodyLength++;
}

void Snake::render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, head.getColor().r, head.getColor().g, head.getColor().b, 0xFF);
	SDL_RenderFillRect(renderer, head.getRect());

	SDL_SetRenderDrawColor(renderer, bodyColor.r, bodyColor.g, bodyColor.b, 0xFF);
	
	for (uint i = 1; i < body.size(); i++) {
		//body[i].render(renderer);
		SDL_RenderFillRect(renderer, body[i].getRect());
	}
}

void Snake::setSnakeColor(SDL_Color color) {
	bodyColor = color;
}

void Snake::setBody(GameWorld* world) {
	int player1Row, player1Col;
	player1Row = 20;
	player1Col = 4;
	Cell* CellPtr = world->getCell(player1Row, player1Col);
	CellPtr->setOccupied(true);
	gridPosition.row = player1Row;
	gridPosition.column = player1Col;

	SDL_Rect* elementRect = CellPtr->getRect();

	head = Cell();
	head.setColor({ 255,255,255 });
	head.setPosition(elementRect->x, elementRect->y);
	head.setDimensions(elementRect->w);
	head.setOccupied(true);
	body.push_back(head);

	Cell bodyCell = Cell();
	CellPtr = world->getCell(21, 4);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	bodyCell.setOccupied(true);
	body.push_back(bodyCell);

	CellPtr = world->getCell(22, 4);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	body.push_back(bodyCell);
}

void Snake::setBody(GameWorld* world, playerNum player2) {
	int player2Row, player2Col;
	player2Row = 4;
	player2Col = 20;
	Cell* CellPtr = world->getCell(player2Row, player2Col);
	CellPtr->setOccupied(true);
	gridPosition.row = player2Row;
	gridPosition.column = player2Col;

	SDL_Rect* elementRect = CellPtr->getRect();

	head = Cell();
	head.setColor({ 255,255,255 });
	head.setPosition(elementRect->x, elementRect->y);
	head.setDimensions(elementRect->w);
	head.setOccupied(true);
	body.push_back(head);

	Cell bodyCell = Cell();
	CellPtr = world->getCell(4, 21);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	//bodyCell.setOccupied(true);
	body.push_back(bodyCell);

	CellPtr = world->getCell(4, 22);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	body.push_back(bodyCell);
}

void Snake::setSpeed(uint sp) {
	speed = sp;
}

void Snake::setDirection(Direction dir) {
	direction = dir;
}

void Snake::setDesiredDirection(Direction dir) {
	desiredDirection = dir;
}

void Snake::increaseSpeed(uint inc) {
	speed += inc;
}

void Snake::setWallCollision(bool collidedWithWall) {
	wallCollision = collidedWithWall;
}

std::vector<Cell>* Snake::getBody() { return &body; }

SDL_Point Snake::getHeadPosition() {
	SDL_Rect* headRect = head.getRect();
	SDL_Point headPoint = { headRect->x, headRect->y };

	return headPoint;
}

GridPosition Snake::getGridPosition() {
	return gridPosition;
}

Direction Snake::getDirection() {
	return direction;
}

void Snake::setSnakePosition(SDL_Point pos)  {
	headPosition.x = pos.x;
	headPosition.y = pos.y;
}

std::string Snake::getName() {
	return name;
}

bool Snake::getWallCollision() {
	return wallCollision;
}

uint Snake::getSpeed() {
	return speed;
}

PickUp::PickUp() {
	puPosition = { 0,0 };
	Cell cell = Cell();
}

void PickUp::render(SDL_Renderer* renderer) {

}

void PickUp::handle(Snake snek) {

}

Cell PickUp::getCell() {
	return puCell;
}

SizeUp::SizeUp() {
	puPosition = { 0,0 };
	Cell cell = Cell();
}

SizeUp::SizeUp(SDL_Color szColor, Cell* cell) {
	puCell.setColor(szColor);
	SDL_Rect* puRect = puCell.getRect();
	SDL_Rect* cellRect = cell->getRect();
	puRect->x = cellRect->x;
	puRect->y = cellRect->y;
	puRect->w = cellRect->w;
	puRect->h = cellRect->h;
	puCell.setDimensions(puRect->h);
}

void SizeUp::handle(Snake snek) {
	snek.increaseSize();
}

void SizeUp::render(SDL_Renderer* renderer) {
	SDL_Color pickupColor = puCell.getColor();
	SDL_SetRenderDrawColor(renderer, pickupColor.r, pickupColor.g, pickupColor.b, 0xFF);
	SDL_RenderFillRect(renderer, puCell.getRect());
}

Cell SizeUp::getCell() {
	return puCell;
}
