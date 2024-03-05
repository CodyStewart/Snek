#include "actor.h"

bool checkBodyCollisions(Snake snake) {
	std::vector<Cell>* snakeBody = snake.getBody();
	Cell snakeHead = snakeBody->at(0);
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
		spriteSheet = nullptr;
		headPosition = getHeadPosition();
		speed = 5;
		bodyLength = (uint)body.size();
		direction = UP;
		desiredDirection = UP;
		wallCollision = false;
		flipMode = 0;
	}
	else {
		setBody(world, player2);
		bodyColor = { 255,255,0 };
		name = snakeName;
		spriteSheet = nullptr;
		headPosition = getHeadPosition();
		speed = 5;
		bodyLength = (uint)body.size();
		direction = LEFT;
		desiredDirection = LEFT;
		wallCollision = false;
		flipMode = 0;
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
	Cell thisBodyCell;

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
					thisBodyCell = body[bodyIndex];
					GridPosition nextCellGP = tempBodyCell.getGridPosition();
					GridPosition thisCellGP = thisBodyCell.getGridPosition();
					if (nextCellGP.column > thisCellGP.column) // the next cell is to the right, update grid position column up one
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column + 1);
					else if (nextCellGP.column < thisCellGP.column)
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column - 1);
					else if (nextCellGP.row > thisCellGP.row)
						body[bodyIndex].setGridPosition(thisCellGP.row + 1, thisCellGP.column);
					else
						body[bodyIndex].setGridPosition(thisCellGP.row - 1, thisCellGP.column);

					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head up one cell
				head.setPosition(head.getRect()->x, head.getRect()->y - head.getLength());
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position up one
				GridPosition headGP = body[0].getGridPosition();
				body[0].setGridPosition(headGP.row - 1, headGP.column);
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
					thisBodyCell = body[bodyIndex];
					GridPosition nextCellGP = tempBodyCell.getGridPosition();
					GridPosition thisCellGP = thisBodyCell.getGridPosition();
					if (nextCellGP.column > thisCellGP.column) // the next cell is to the right, update grid position column up one
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column + 1);
					else if (nextCellGP.column < thisCellGP.column)
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column - 1);
					else if (nextCellGP.row > thisCellGP.row)
						body[bodyIndex].setGridPosition(thisCellGP.row + 1, thisCellGP.column);
					else
						body[bodyIndex].setGridPosition(thisCellGP.row - 1, thisCellGP.column);

					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head down one cell
				head.setPosition(head.getRect()->x, head.getRect()->y + head.getLength());
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position down one
				GridPosition headGP = body[0].getGridPosition();
				body[0].setGridPosition(headGP.row + 1, headGP.column);
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
					thisBodyCell = body[bodyIndex];
					GridPosition nextCellGP = tempBodyCell.getGridPosition();
					GridPosition thisCellGP = thisBodyCell.getGridPosition();
					if (nextCellGP.column > thisCellGP.column) // the next cell is to the right, update grid position column up one
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column + 1);
					else if (nextCellGP.column < thisCellGP.column)
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column - 1);
					else if (nextCellGP.row > thisCellGP.row)
						body[bodyIndex].setGridPosition(thisCellGP.row + 1, thisCellGP.column);
					else
						body[bodyIndex].setGridPosition(thisCellGP.row - 1, thisCellGP.column);

					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head left one cell
				head.setPosition(head.getRect()->x - head.getWidth(), head.getRect()->y);
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position left one
				GridPosition headGP = body[0].getGridPosition();
				body[0].setGridPosition(headGP.row, headGP.column - 1);
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
					thisBodyCell = body[bodyIndex];
					GridPosition nextCellGP = tempBodyCell.getGridPosition();
					GridPosition thisCellGP = thisBodyCell.getGridPosition();
					if (nextCellGP.column > thisCellGP.column) // the next cell is to the right, update grid position column up one
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column + 1);
					else if (nextCellGP.column < thisCellGP.column)
						body[bodyIndex].setGridPosition(thisCellGP.row, thisCellGP.column - 1);
					else if (nextCellGP.row > thisCellGP.row)
						body[bodyIndex].setGridPosition(thisCellGP.row + 1, thisCellGP.column);
					else
						body[bodyIndex].setGridPosition(thisCellGP.row - 1, thisCellGP.column);

					body[bodyIndex].setPosition(tempBodyCell.getRect()->x, tempBodyCell.getRect()->y);
				}

				// move the head right one cell
				head.setPosition(head.getRect()->x + head.getWidth(), head.getRect()->y);
				// finally update the first element of the body to the head
				body[0].setPosition(head.getRect()->x, head.getRect()->y);

				// move the grid position right one
				GridPosition headGP = body[0].getGridPosition();
				body[0].setGridPosition(headGP.row, headGP.column + 1);
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

	if (flipMode == 0)
		flipMode = 1;
	else
		flipMode = 0;
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
		newCell.setGridPosition(body.at(body.size() - 1).getGridPosition().row, body.at(body.size() - 1).getGridPosition().column - 1);
	}
	else if (tailCellRect->x > penultCellRect->x) {
		// tail is to the right of penultimate cell, add new cell to the right of tail
		newCell.setPosition(tailCellRect->x + tailCellRect->w, tailCellRect->y);
		GridPosition gp = gameWorld.convertCoordsToRowCol(tailCellRect->x + tailCellRect->w, tailCellRect->y);
		gameWorld.getCell(gp.row, gp.column)->setOccupied(true);
		newCell.setGridPosition(body.at(body.size() - 1).getGridPosition().row, body.at(body.size() - 1).getGridPosition().column + 1);
	}
	else if (tailCellRect->y < penultCellRect->y) {
		// tail is above penultimate cell, add new cell above tail
		newCell.setPosition(tailCellRect->x, tailCellRect->y - tailCellRect->h);
		GridPosition gp = gameWorld.convertCoordsToRowCol(tailCellRect->x, tailCellRect->y - tailCellRect->h);
		gameWorld.getCell(gp.row, gp.column)->setOccupied(true);
		newCell.setGridPosition(body.at(body.size() - 1).getGridPosition().row - 1, body.at(body.size() - 1).getGridPosition().column);
	}
	else if (tailCellRect->y > penultCellRect->y) {
		// tail is below penultimate cell, add new cell below tail
		newCell.setPosition(tailCellRect->x, tailCellRect->y + tailCellRect->h);
		GridPosition gp = gameWorld.convertCoordsToRowCol(tailCellRect->x, tailCellRect->y + tailCellRect->h);
		gameWorld.getCell(gp.row, gp.column)->setOccupied(true);
		newCell.setGridPosition(body.at(body.size() - 1).getGridPosition().row + 1, body.at(body.size() - 1).getGridPosition().column);
	}
	else { // we should not be here
		printf("ERROR! increaseSize() function has entered a section of code it should not be in!\n");
	}

	newCell.setOccupied(true);
	body.push_back(newCell);
	bodyLength++;
}

bool cellIsACorner(GridPosition firstGP, GridPosition secondGP) {
	if (firstGP.row == secondGP.row || firstGP.column == secondGP.column)
		return false;
	else
		return true;
}

void renderCorrectCorner(SDL_Renderer* renderer, GridPosition previousGP, GridPosition nextGP, GridPosition thisGP, SDL_Rect* cellRect, SpriteSheet* spriteSheet) {
	SDL_RendererFlip flip;
	int angleOfRotation = 0;
	if (previousGP.row > nextGP.row) {
		if (previousGP.column > nextGP.column) {
			if (thisGP.row == previousGP.row) {
				flip = SDL_FLIP_VERTICAL;
				spriteSheet->render(renderer, 2, cellRect, angleOfRotation, &flip);
			}
			else {
				angleOfRotation = 90;
				spriteSheet->render(renderer, 2, cellRect, angleOfRotation);
			}
		}
		else {
			if (thisGP.row == previousGP.row) {
				angleOfRotation = 180;
				spriteSheet->render(renderer, 2, cellRect, angleOfRotation);
			}
			else {
				flip = SDL_FLIP_VERTICAL;
				angleOfRotation = 90;
				spriteSheet->render(renderer, 2, cellRect, angleOfRotation, &flip);
			}
		}
	}
	else if (previousGP.row < nextGP.row) {
		if (previousGP.column > nextGP.column) {
			if (thisGP.row == previousGP.row) {
				spriteSheet->render(renderer, 2, cellRect);
			}
			else {
				flip = SDL_FLIP_HORIZONTAL;
				angleOfRotation = 90;
				spriteSheet->render(renderer, 2, cellRect, angleOfRotation, &flip);
			}
		}
		else {
			if (thisGP.row == previousGP.row) {
				flip = SDL_FLIP_HORIZONTAL;
				spriteSheet->render(renderer, 2, cellRect, angleOfRotation, &flip);
			}
			else {
				angleOfRotation = 270;
				spriteSheet->render(renderer, 2, cellRect, angleOfRotation);
			}
		}
	}
}

void Snake::render(SDL_Renderer* renderer) {
	int angleOfRotation = 0;

	// render the head
	SDL_Rect* headRect = head.getRect();
	if (direction == UP)
		angleOfRotation = 0;
	else if (direction == RIGHT)
		angleOfRotation = 90;
	else if (direction == DOWN)
		angleOfRotation = 180;
	else
		angleOfRotation = 270;
	spriteSheet->render(renderer, 0, headRect, angleOfRotation);

	GridPosition previousCellGP;
	GridPosition thisCellGP;
	GridPosition nextCellGP;

	// render the body
	SDL_Rect* currentBodyRect;
	for (int i = 1; i < body.size() - 1; i++) {
		currentBodyRect = body.at(i).getRect();

		previousCellGP = body.at(i - 1).getGridPosition();
		thisCellGP = body.at(i).getGridPosition();
		nextCellGP = body.at(i + 1).getGridPosition();
		if (cellIsACorner(previousCellGP, nextCellGP)) {
			renderCorrectCorner(renderer, previousCellGP, nextCellGP, thisCellGP, currentBodyRect, spriteSheet);
		}
		else {
			if (previousCellGP.column > thisCellGP.column)
				angleOfRotation = 90;
			else if (previousCellGP.column < thisCellGP.column)
				angleOfRotation = 270;
			else if (previousCellGP.row > thisCellGP.row)
				angleOfRotation = 180;
			else
				angleOfRotation = 0;
			spriteSheet->render(renderer, 1, currentBodyRect, angleOfRotation);
		}
	}

	// finally, render the tail
	currentBodyRect = body.at(body.size() - 1).getRect();
	previousCellGP = body.at(body.size() - 2).getGridPosition();
	thisCellGP = body.at(body.size() - 1).getGridPosition();
	if (previousCellGP.column > thisCellGP.column)
		angleOfRotation = 90;
	else if (previousCellGP.column < thisCellGP.column)
		angleOfRotation = 270;
	else if (previousCellGP.row > thisCellGP.row)
		angleOfRotation = 180;
	else
		angleOfRotation = 0;
	spriteSheet->render(renderer, 3, currentBodyRect, angleOfRotation);
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
	head.setGridPosition(player1Row, player1Col);
	body.push_back(head);

	Cell bodyCell = Cell();
	CellPtr = world->getCell(21, 4);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	bodyCell.setOccupied(true);
	bodyCell.setGridPosition(player1Row + 1, player1Col);
	body.push_back(bodyCell);

	CellPtr = world->getCell(22, 4);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	bodyCell.setGridPosition(player1Row + 2, player1Col);
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
	head.setGridPosition(player2Row, player2Col);
	body.push_back(head);

	Cell bodyCell = Cell();
	CellPtr = world->getCell(4, 21);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	bodyCell.setGridPosition(player2Row, player2Col + 1);
	body.push_back(bodyCell);

	CellPtr = world->getCell(4, 22);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	bodyCell.setGridPosition(player2Row, player2Col + 2);
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

void Snake::setSnakeTexture(SpriteSheet* snakeTexture) {
	spriteSheet = snakeTexture;
}

Cell* Snake::getHead() { return &head; }
std::vector<Cell>* Snake::getBody() { return &body; }
SDL_Color Snake::getColor() { return bodyColor; }

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

GridPosition PickUp::getPosition() {
	return puPosition;
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
	puCell.setGridPosition(cell->getGridPosition().row, cell->getGridPosition().column);
	puPosition.column = cell->getGridPosition().column;
	puPosition.row = cell->getGridPosition().row;
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

GridPosition SizeUp::getPosition() {
	return puPosition;
}