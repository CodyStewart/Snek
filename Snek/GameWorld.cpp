#include "GameWorld.h"
#include "actor.h"

Uint32 getPickupGenerationTime() {
	Uint32 initialPickupGenerationTime = 5000;
	Uint32 generationTimeCoeff = gameScore / 3;
	
	if (generationTimeCoeff * 500 >= initialPickupGenerationTime || initialPickupGenerationTime - generationTimeCoeff * 500 < 2000)
		initialPickupGenerationTime = 2000;
	else 
		initialPickupGenerationTime -= generationTimeCoeff * 500;

	return initialPickupGenerationTime;
}

Cell::Cell() {
	cellTexture = nullptr;
	cellColor = { 255,255,255 };
	cellBlock = {};

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

Texture* Cell::getTexture() { return cellTexture; }
SDL_Color Cell::getColor() { return cellColor; }
SDL_Rect* Cell::getRect() { return &cellBlock; }
int Cell::getWidth() { return width; }
int Cell::getLength() { return length; }
bool Cell::getOccupied() { return occupied; }

GameWorld::GameWorld(int unitDist, const int numRows, const int numCols) {
	unitDistance = unitDist;
	numOfRows = numRows;
	numOfCols = numCols;

	//grid = new Cell [numRows * numCols];
	//grid = &gridPtr;

	int XOffset = (CURRENT_SCREEN_WIDTH / 2) - (numOfCols * unitDistance) / 2;

	// generate the grid
	for (int row = 0; row < numOfRows; row++) {
		for (int col = 0; col < numOfCols; col++) {
			grid[row][col].setPosition(col * unitDistance + XOffset, row * unitDistance + paddingY);
			grid[row][col].setDimensions(unitDistance);
			/*grid[numOfRows * row + col].setPosition(col * unitDistance + XOffset, row * unitDistance);
			grid[numOfRows * row + col].setDimensions(unitDistance);*/
		}
	}
}

void GameWorld::render(SDL_Renderer* renderer) {
	for (int row = 0; row < numOfRows; row++) {
		for (int col = 0; col < numOfCols; col++) {
			// render each cell
			//grid[numOfRows * row + col].render(renderer);
			grid[row][col].render(renderer);
		}
	}
}

void GameWorld::generatePickups(Uint32 delta) {
	static Uint32 accumulatedTime = 0;
	accumulatedTime += delta;

	if (accumulatedTime >= getPickupGenerationTime()) {
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

	// get the column and row number based on the coords provided
	int colNum = (x - grid[0][0].getRect()->x) / unitDistance; // we use the first cell in the grid to find our x and y screen padding
	int rowNum = (y - grid[0][0].getRect()->y) / unitDistance;

	gp.column = colNum;
	gp.row = rowNum;

	return gp;
}

Cell* GameWorld::getGrid() {
	return *grid;
}

Cell* GameWorld::getCell(int row, int col) {
	// return cell at row, column
	return &grid[row][col];
}
