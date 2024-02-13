#include "actor.h"

bool checkBodyCollisions(Snake snake) {
	std::vector<Cell>* snakeBody = snake.getBody();
	for (int firstCellCompared = 0; firstCellCompared < snakeBody->size() - 1; firstCellCompared++) {
		for (int secondCellCompared = firstCellCompared + 1; secondCellCompared < snakeBody->size() - 1; secondCellCompared++) {
			if ((*snakeBody)[firstCellCompared].getRect()->x == (*snakeBody)[secondCellCompared].getRect()->x && (*snakeBody)[firstCellCompared].getRect()->y == (*snakeBody)[secondCellCompared].getRect()->y)
				return true;
		}
	}

	return false;
}

void checkPickUpCollisions(Snake snake) {

}


Snake::Snake(GameWorld* world) {
	setBody(world);
	headTexture = nullptr;
	bodyColor = { 0, 0, 255 };

	headPosition = getHeadPosition();
	speed = 5;
	bodyLength = (uint)body.size();
	direction = UP;
	desiredDirection = UP;
}

void Snake::move(Uint32 deltaT, GameWorld* gameWorld) {
	// check if enough time has past for the snake to have moved
	static Uint32 accumulatedTime = 0;
	Uint32 timeToMove = 1000 / speed;
	accumulatedTime += deltaT;

	if (accumulatedTime >= timeToMove) {
		accumulatedTime = 0;
		moveByOne(gameWorld);
		increaseSize();
	}
}

void Snake::moveByOne(GameWorld* gameWorld) {
	// move the snake one space based on direction
	Cell tempBodyCell;

	//// unset the cell uncovered by moving the tail
	//Cell tailCell = body[body.size() - 1];
	//GridPosition gp = gameWorld->convertCoordsToRowCol(tailCell.getRect()->x, tailCell.getRect()->y);
	//Cell* gridCellUncovered = gameWorld->getCell(gp.row, gp.column);
	//gridCellUncovered->setOccupied(false);

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

bool Snake::checkCollisions() {
	if (checkBodyCollisions(*this)) {
		printf("Body has collided with itself!\n");
		return true;
	}
	//checkPickUpCollisions();

	return false;
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
	Cell* CellPtr = world->getCell(12, 4);
	CellPtr->setOccupied(true);
	gridPosition.row = 12;
	gridPosition.column = 4;

	SDL_Rect* elementRect = CellPtr->getRect();

	head = Cell();
	head.setColor({ 255,255,255 });
	head.setPosition(elementRect->x, elementRect->y);
	head.setDimensions(elementRect->w);
	head.setOccupied(true);
	body.push_back(head);

	Cell bodyCell = Cell();
	CellPtr = world->getCell(13, 4);
	CellPtr->setOccupied(true);
	elementRect = CellPtr->getRect();
	bodyCell.setPosition(elementRect->x, elementRect->y);
	bodyCell.setDimensions(elementRect->w);
	bodyCell.setOccupied(true);
	body.push_back(bodyCell);

	CellPtr = world->getCell(14, 4);
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

PickUp::PickUp() {
	puPosition = { 0,0 };
	Cell cell = Cell();
}

void PickUp::render(SDL_Renderer* renderer) {

}

void PickUp::handle(Snake snek) {

}

//Cell PickUp::getCell() {
//
//}

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

//Cell SizeUp::getCell() {
//	return puCell;
//}
