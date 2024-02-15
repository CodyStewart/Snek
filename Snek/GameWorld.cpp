#include "GameWorld.h"
#include "actor.h"

Uint32 getPickupGenerationTime(GameWorld* world) {
	Uint32 generationTime = 0;

	if (world->getTimeSinceStartOfProgram() < 10000)
		return 5000;
	else if (world->getTimeSinceStartOfProgram() < 25000)
		return 4000;
	else if (world->getTimeSinceStartOfProgram() < 40000)
		return 3000;
	else
		return 2000;
}

Cell::Cell() {
	cellTexture = nullptr;
	cellColor = { 255,255,255 };
	cellBlock = {};

	cellGridPosition = { 0,0 };

	width = length = 0;
	occupied = false;
}

void Cell::render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0x00, 0x80, 0x00, 0xFF);
	SDL_RenderDrawRect(renderer, &cellBlock);
}

void Cell::setColor(SDL_Color color) {
	cellColor = color;
}

void Cell::setTexture(Texture* texture) {
	cellTexture = texture;
}

void Cell::setPosition(int x, int y) {
	cellBlock.x = x;
	cellBlock.y = y;
}

void Cell::setDimensions(int dim) {
	width = length = dim;
	cellBlock.w = cellBlock.h = dim;
}

void Cell::setOccupied(bool isOccupied) {
	occupied = isOccupied;
}

void Cell::setGridPosition(int row, int col) {
	cellGridPosition = { row, col };
}

Texture* Cell::getTexture() { return cellTexture; }
SDL_Color Cell::getColor() { return cellColor; }
SDL_Rect* Cell::getRect() { return &cellBlock; }
int Cell::getWidth() { return width; }
int Cell::getLength() { return length; }
bool Cell::getOccupied() { return occupied; }
GridPosition Cell::getGridPosition() { return cellGridPosition; }

GameWorld::GameWorld(int unitDist, const int numRows, const int numCols) {
	unitDistance = unitDist;
	numOfRows = numRows;
	numOfCols = numCols;

	timeSinceStartOfProgram.start();

	int XOffset = (CURRENT_SCREEN_WIDTH / 2) - (numOfCols * unitDistance) / 2;

	// generate the grid
	for (int row = 0; row < numOfRows; row++) {
		for (int col = 0; col < numOfCols; col++) {
			grid[row][col].setPosition(col * unitDistance + XOffset, row * unitDistance + paddingY);
			grid[row][col].setDimensions(unitDistance);
			grid[row][col].setGridPosition(row, col);
		}
	}
}

void GameWorld::render(SDL_Renderer* renderer) {
	for (int row = 0; row < numOfRows; row++) {
		for (int col = 0; col < numOfCols; col++) {
			// render each cell
			grid[row][col].render(renderer);
		}
	}
}

Uint32 GameWorld::generatePickups(Uint32 delta) {
	static Uint32 accumulatedTime = 0;
	accumulatedTime += delta;

	if (accumulatedTime >= getPickupGenerationTime(this)) {
		accumulatedTime = 0;

		srand(SDL_GetTicks());
		int randCol, randRow;

		randCol = rand() % NUMOFCOLS;
		randRow = rand() % NUMOFROWS;
		int maxThreshold = 100;
		int loopCount = 0;
		bool cellIsOccupied = gameWorld.getCell(randRow, randCol)->getOccupied();
		while (cellIsOccupied && loopCount < maxThreshold) {
			randCol = rand() % NUMOFCOLS;
			randRow = rand() % NUMOFROWS;
			cellIsOccupied = gameWorld.getCell(randRow, randCol)->getOccupied();
			loopCount++;
		}

		SDL_Color Red = { 255, 0, 0 };

		Cell* puCell = &grid[randRow][randCol];

		SizeUp* sizeup = new SizeUp(Red, puCell);

		pickupGathering.push_back(sizeup);
	}
	
	return getPickupGenerationTime(this);
}

void GameWorld::resizeGameWorld(int newSize) {
	// TODO: need to resize any textures
	unitDistance = newSize;

	int XOffset = (CURRENT_SCREEN_WIDTH / 2) - (numOfCols * unitDistance) / 2;

	for (int row = 0; row < numOfRows; row++) {
		for (int col = 0; col < numOfCols; col++) {
			// resize each cell
			grid[row][col].setPosition(col * newSize + XOffset, row * newSize + paddingY);
			grid[row][col].setDimensions(newSize);
		}
	}
}

GridPosition GameWorld::convertCoordsToRowCol(int x, int y) {
	GridPosition gp;
	gp.column = 0;
	gp.row = 0;

	// get the column and row number based on the coords provided
	int colNum = (x - grid[0][0].getRect()->x) / unitDistance; // we use the first cell in the grid to find our x and y screen padding
	int rowNum = (y - grid[0][0].getRect()->y) / unitDistance;

	gp.column = colNum;
	gp.row = rowNum;

	return gp;
}

int GameWorld::getNumOfRows() {
	return numOfRows;
}

int GameWorld::getNumOfCols() {
	return numOfRows;
}

Cell* GameWorld::getGrid() {
	return *grid;
}

Cell* GameWorld::getCell(int row, int col) {
	// return cell at row, column
	return &grid[row][col];
}

Uint32 GameWorld::getTimeSinceStartOfProgram() { return timeSinceStartOfProgram.getTicks(); }