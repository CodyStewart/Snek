#include "GameWorld.h"
#include "actor.h"

Cell::Cell() {
	cellTexture = nullptr;
	cellColor = { 255,255,255 };
	cellBlock = {};

	width = length = 0;
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

Texture* Cell::getTexture() { return cellTexture; }
SDL_Color Cell::getColor() { return cellColor; }
SDL_Rect* Cell::getRect() { return &cellBlock; }
int Cell::getWidth() { return width; }
int Cell::getLength() { return length; }

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

	if (accumulatedTime >= 5000) {
		accumulatedTime = 0;

		srand(SDL_GetTicks());
		int randCol, randRow;
		randCol = rand() % NUMOFCOLS;
		randRow = rand() % NUMOFROWS;

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

Cell* GameWorld::getGrid() {
	return *grid;
}

Cell* GameWorld::getCell(int row, int col) {
	// return cell at row, column
	return &grid[row][col];
}
