#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>
#include <sstream>

#include "snek.h"
#include "gameWindow.h"
#include "GameWorld.h"
#include "texture.h"
#include "timer.h"
#include "actor.h"
#include "menu.h"
#include "ai.h"
#include "graphGenerator.h"
#include "animation.h"

bool init();

bool loadMedia();

void close();

GameWindow* window = new GameWindow();

SDL_Renderer* renderer = NULL;

const int DEFAULT_SCREEN_WIDTH = 1920;
const int DEFAULT_SCREEN_HEIGHT = 1080;
const int paddingX = 5;
const int paddingY = 10;

int CURRENT_SCREEN_WIDTH = DEFAULT_SCREEN_WIDTH;
int CURRENT_SCREEN_HEIGHT = DEFAULT_SCREEN_HEIGHT;

uint UNIT_DISTANCE = (DEFAULT_SCREEN_HEIGHT - paddingY) / NUMOFCOLS;

bool twoPlayerMode = false;
bool endGameState = false;
bool menuIsOpen = false;

bool inStartMenu = true;
bool inMainMenu = false;
bool inOnePlayerSubMenu = false;
bool inTwoPlayerSubMenu = false;
bool inGame = false;

bool aiControlsPlayer1 = false;
bool aiControlsPlayer2 = false;
bool windowResized = false;
bool runGame = true;
bool beginGame = false;
uint gameScore = 0;

GameWorld gameWorld = GameWorld(UNIT_DISTANCE, NUMOFROWS, NUMOFCOLS);
Snake snake = Snake(&gameWorld, player1, "Player 1");
Snake rival = Snake(&gameWorld, player2, "Player 2");
AI ai = AI();

std::vector<PickUp*> pickupGathering;

TTF_Font* DefaultFont = NULL;
TTF_Font* pixelFont = NULL;

TextTexture* tb = new TextTexture();
TextTexture* winLoseText = new TextTexture();
TextTexture* score = new TextTexture();
TextTexture* player1SpeedText = new TextTexture();
TextTexture* player2SpeedText = new TextTexture();
TextTexture* pickupGenerationSpeedText = new TextTexture();
TextTexture* startMenuText = new TextTexture();

Menu* startMenu;
Menu* mainMenu;
Menu* onePlayerSubMenu;
Menu* twoPlayerSubMenu;
Menu* inGameMenu;

Texture mainMenuTexture = Texture();
SpriteSheet player1Sheet = SpriteSheet();
SpriteSheet player2Sheet = SpriteSheet();

Animation player1Appear = Animation();
Animation player1Idle = Animation();
Animation player1Quizical = Animation();
Animation player1Select = Animation();
Animation player1Unimpressed = Animation();
Animation player2Appear = Animation();
Animation player2Idle = Animation();
Animation player2Quizical = Animation();
Animation player2Select = Animation();
Animation player2Angry = Animation();

Mix_Music* music;
Mix_Chunk* player1Sound;
Mix_Chunk* player2Sound;

void player1FallingAnimation(Snake* snek) {
	uint timeOfAnimation = 2000;
	static int numOfTimesCalled = 1;
	int pixelSize = 0;

	Cell* snakeHead = snek->getHead();
	SDL_Rect* headRect = snakeHead->getRect();
	static int zoomFactor = 2 * headRect->w;
	pixelSize = zoomFactor;
	SDL_Rect temRect;
	temRect.x = headRect->x;
	temRect.y = headRect->y;
	temRect.w = headRect->w;
	temRect.h = headRect->h;

	temRect.w = zoomFactor;
	temRect.h = zoomFactor;
	temRect.x -= (temRect.w - headRect->w) / 2;
	temRect.y -= (temRect.h - headRect->h) / 2;

	std::vector<Cell>* snakeBody = snek->getBody();
	SDL_Color snakeColor = snek->getColor();
	SDL_Color snakeHeadColor = snek->getHead()->getColor();

	SDL_SetRenderDrawColor(renderer, snakeHeadColor.r, snakeHeadColor.g, snakeHeadColor.b, 0xFF);
	SDL_RenderFillRect(renderer, &temRect);

	SDL_SetRenderDrawColor(renderer, snakeColor.r, snakeColor.g, snakeColor.b, 0xFF);


 	for (uint i = 1; i < snakeBody->size(); i++) {
		//body[i].render(renderer);
		SDL_Rect* snakeRect = snakeBody->at(i).getRect();
		SDL_Rect tempRect;
		tempRect.x = snakeRect->x;
		tempRect.y = snakeRect->y;
		tempRect.w = snakeRect->w;
		tempRect.h = snakeRect->h;

		tempRect.w = zoomFactor;
		tempRect.h = zoomFactor;
		tempRect.x = temRect.x;
		tempRect.y = temRect.y + (temRect.h * i);
		SDL_RenderFillRect(renderer, &tempRect);
	}

	zoomFactor -= 1 * (numOfTimesCalled / 10);
	numOfTimesCalled++;
	if (numOfTimesCalled > 10)
		numOfTimesCalled = 0;

	//if (zoomFactor <= snakeHead->getRect()->w)
	beginGame = true;
}

void player2FallingAnimation(Snake* snek) {
	uint timeOfAnimation = 2000;
	static int numOfTimesCalled = 1;
	int pixelSize = 0;

	Cell* snakeHead = snek->getHead();
	SDL_Rect* headRect = snakeHead->getRect();
	static int zoomFactor = 2 * headRect->w;
	pixelSize = zoomFactor;
	SDL_Rect temRect;
	temRect.x = headRect->x;
	temRect.y = headRect->y;
	temRect.w = headRect->w;
	temRect.h = headRect->h;

	temRect.w = zoomFactor;
	temRect.h = zoomFactor;
	temRect.x -= (temRect.w - headRect->w) / 2;
	temRect.y -= (temRect.h - headRect->h) / 2;

	std::vector<Cell>* snakeBody = snek->getBody();
	SDL_Color snakeColor = snek->getColor();
	SDL_Color snakeHeadColor = snek->getHead()->getColor();

	SDL_SetRenderDrawColor(renderer, snakeHeadColor.r, snakeHeadColor.g, snakeHeadColor.b, 0xFF);
	SDL_RenderFillRect(renderer, &temRect);

	SDL_SetRenderDrawColor(renderer, snakeColor.r, snakeColor.g, snakeColor.b, 0xFF);


	for (uint i = 1; i < snakeBody->size(); i++) {
		//body[i].render(renderer);
		SDL_Rect* snakeRect = snakeBody->at(i).getRect();
		SDL_Rect tempRect;
		tempRect.x = snakeRect->x;
		tempRect.y = snakeRect->y;
		tempRect.w = snakeRect->w;
		tempRect.h = snakeRect->h;

		tempRect.w = zoomFactor;
		tempRect.h = zoomFactor;
		tempRect.x = temRect.x + (temRect.w * i);
		tempRect.y = temRect.y;
		SDL_RenderFillRect(renderer, &tempRect);
	}

	zoomFactor -= 1 * (numOfTimesCalled / 10);
	numOfTimesCalled++;
	if (numOfTimesCalled > 10)
		numOfTimesCalled = 0;

	if (zoomFactor <= snakeHead->getRect()->w)
		beginGame = true;
}

void restartGame() {
	gameScore = 0;
	endGameState = false;
	gameWorld.reset();

	snake.getBody()->clear();
	snake.setBody(&gameWorld);
	snake.setDirection(UP);
	snake.setDesiredDirection(UP);
	snake.setSpeed(5);

	ai.reset();

	if (twoPlayerMode) {
		rival.getBody()->clear();
		rival.setBody(&gameWorld, player2);
		rival.setDirection(LEFT);
		rival.setDesiredDirection(LEFT);
		rival.setSpeed(5);
	}

	pickupGathering.clear();
	menuIsOpen = false;
}

void QuitGame() {
	runGame = false;
}

void OnePlayerSetup() {
	inOnePlayerSubMenu = true;
	inMainMenu = false;

	player1Idle.stopAnimation();
	player1Quizical.startAnimation();
}

void OnePlayerStartHuman() {
	//inOnePlayerSubMenu = false;
	//inGame = true;
	aiControlsPlayer1 = false;

	player1Quizical.stopAnimation();
	player2Idle.stopAnimation();
	player1Select.startAnimation();
}

void OnePlayerStartAI() {
	inOnePlayerSubMenu = false;
	inGame = true;
	aiControlsPlayer1 = true;

	player1Quizical.stopAnimation();
	player2Idle.stopAnimation();
	player1Select.startAnimation();
}

void BackToMainMenu() {
	inOnePlayerSubMenu = false;
	inTwoPlayerSubMenu = false;
	inMainMenu = true;

	player1Quizical.stopAnimation();
	player1Idle.startAnimation();
}

void TwoPlayerSetup() {
	inTwoPlayerSubMenu = true;
	inMainMenu = false;

	player1Idle.stopAnimation();
	player2Idle.stopAnimation();

	player1Quizical.startAnimation();
	player2Quizical.startAnimation();
}

void TwoPlayerStartHuman() {
	//twoPlayerMode = true;
	//inTwoPlayerSubMenu = false;
	//inGame = true;
	aiControlsPlayer1 = false;

	player1Quizical.stopAnimation();
	player2Quizical.stopAnimation();
	player1Select.startAnimation();
	player2Select.startAnimation();
}

void TwoPlayerStartOneAI() {
	twoPlayerMode = true;
	inTwoPlayerSubMenu = false;
	inGame = true;
	aiControlsPlayer1 = false;
	aiControlsPlayer2 = true;

	player1Quizical.stopAnimation();
	player2Quizical.stopAnimation();
}

void TwoPlayerStartTwoAI() {
	twoPlayerMode = true;
	inTwoPlayerSubMenu = false;
	inGame = true;
	aiControlsPlayer1 = true;
	aiControlsPlayer2 = true;

	player1Quizical.stopAnimation();
	player2Quizical.stopAnimation();
}

void DrawMainMenu(SDL_Renderer* renderer) {
	/*SDL_Rect mainMenuRect = { CURRENT_SCREEN_WIDTH * 11 / 40, CURRENT_SCREEN_HEIGHT * 7 / 10, CURRENT_SCREEN_WIDTH * 9 / 20, CURRENT_SCREEN_HEIGHT / 4 };
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xEB, 0x3B, 0xFF);
	SDL_RenderDrawRect(renderer, &mainMenuRect);
	SDL_RenderDrawRect(renderer, &mainMenuRect);*/
	mainMenu->render(renderer, { 0,0,0 });
}

void DrawOnePlayerSubMenu(SDL_Renderer* renderer) {
	onePlayerSubMenu->render(renderer, { 0,0,0 });
}

void DrawTwoPlayerSubMenu(SDL_Renderer* renderer) {
	twoPlayerSubMenu->render(renderer, { 0,0,0 });
}

bool init() {
	bool success = true;

	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else {
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		if(!window->init(DEFAULT_SCREEN_WIDTH, DEFAULT_SCREEN_HEIGHT)) {
			printf("Window was not created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			renderer = window->createRenderer();
			if (renderer == NULL) {
				printf("Renderer was not created! SDL_Error: %s\n", SDL_GetError());
				success = false;
			}
			else {
				int imgFlags = IMG_INIT_JPG | IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags)) {
					printf("SDL_image could not initiailize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}

				if (TTF_Init() == -1) {
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool loadMedia() {
	bool success = true;

	// load main menu title
	if (!mainMenuTexture.loadTextureFromFile(renderer, "openingAnimations/snakeTitle3.png")) {
		printf("Failed to load title screen texture!\n");
		success = false;
	}

	// load player 1 textures
	if (!player1Sheet.loadSpriteSheet(renderer, "player1Sprites.png", 64, 64, 4)) {
		printf("Failed to load player 1's textures!\n");
		success = false;
	}

	// load player 2 textures
	if (!player2Sheet.loadSpriteSheet(renderer, "player2Sprites.png", 64, 64, 4)) {
		printf("Failed to load player 2's textures!\n");
		success = false;
	}

	// load player 1 appearance animation
	if (!player1Appear.loadAnimationFromFile(renderer, "openingAnimations/player1AppearAnimation.png")) {
		printf("Failed to load player 1 appearance animation!\n");
		success = false;
	}

	// load player 1 idle animation
	if (!player1Idle.loadAnimationFromFile(renderer, "openingAnimations/player1IdleAnimation.png")) {
		printf("Failed to load player 1 appearance animation!\n");
		success = false;
	}

	// load player 2 appearance animation
	if (!player2Appear.loadAnimationFromFile(renderer, "openingAnimations/player2Appear.png")) {
		printf("Failed to load player 2 appearance animation!\n");
		success = false;
	}

	// load player 2 idle animation
	if (!player2Idle.loadAnimationFromFile(renderer, "openingAnimations/player2IdleAnimation.png")) {
		printf("Failed to load player 2 idle animation!\n");
		success = false;
	}

	// load player 1 quizical animation
	if (!player1Quizical.loadAnimationFromFile(renderer, "openingAnimations/player1Quizical.png")) {
		printf("Failed to load player 2 idle animation!\n");
		success = false;
	}

	// load player 1 select animation
	if (!player1Select.loadAnimationFromFile(renderer, "openingAnimations/player1SelectAnimation.png")) {
		printf("Failed to load player 2 idle animation!\n");
		success = false;
	}

	// load player 1 unimpressed animation
	if (!player1Unimpressed.loadAnimationFromFile(renderer, "openingAnimations/player1UnimpressedAnimation.png")) {
		printf("Failed to load player 2 idle animation!\n");
		success = false;
	}

	// load player 2 quizical animation
	if (!player2Quizical.loadAnimationFromFile(renderer, "openingAnimations/player2QuizicalAnimation.png")) {
		printf("Failed to load player 2 idle animation!\n");
		success = false;
	}

	// load player 2 select animation
	if (!player2Select.loadAnimationFromFile(renderer, "openingAnimations/player2SelectAnimation.png")) {
		printf("Failed to load player 2 idle animation!\n");
		success = false;
	}

	// load player 2 angry animation
	if (!player2Angry.loadAnimationFromFile(renderer, "openingAnimations/player2Angry.png")) {
		printf("Failed to load player 2 idle animation!\n");
		success = false;
	}

	// load music
	music = Mix_LoadMUS("Minus 273 Degrees.wav");
	if (music == NULL) {
		printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
		//success = false;
	}

	// load sound effects
	player1Sound = Mix_LoadWAV("608645__theplax__crunch-5.wav");
	if (player1Sound == NULL) {
		printf("Failed to load player 1's sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		//success = false;
	}
	player2Sound = Mix_LoadWAV("rivalCrunch.wav");
	if (player2Sound == NULL) {
		printf("Failed to load player 2's sound effect! SDL_mixer Error: %s\n", Mix_GetError());
		//success = false;
	}

	DefaultFont = TTF_OpenFont("consola.ttf", 36);
	if (DefaultFont == NULL) {
		printf("TTF could not open font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	pixelFont = TTF_OpenFont("Unibody8Pro-Regular.otf", 36);
	if (pixelFont == NULL) {
		printf("TTF could not open font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void close() {
	Mix_HaltMusic();

	Mix_FreeChunk(player1Sound);
	Mix_FreeChunk(player2Sound);
	Mix_FreeMusic(music);
	player1Sound = NULL;
	player2Sound = NULL;
	music = NULL;

	TTF_CloseFont(DefaultFont);
	DefaultFont = NULL;

	TTF_CloseFont(pixelFont);
	pixelFont = NULL;

	SDL_DestroyRenderer(renderer);
	renderer = NULL;

	window->free();
	free(window);

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_Rect clipRect = SDL_Rect({ 0,100,1000,900 });
	SDL_Rect clipRect3 = SDL_Rect({ 300,100,700,900 });
	
	if (inStartMenu) {
		startMenu->render(renderer, {0,0,0}, &clipRect);
		startMenuText->render(renderer, CURRENT_SCREEN_WIDTH / 2 - startMenuText->getWidth() / 2, CURRENT_SCREEN_HEIGHT * 3 / 5);
	}
	else if (inMainMenu) {
		player1Appear.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player1Appear.getSpriteWidth() / 2, 0, &clipRect);
		player1Idle.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player1Idle.getSpriteWidth() / 2, 0, &clipRect);
		player2Appear.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player1Appear.getSpriteWidth() / 2 + 300, 0, &clipRect3);
		player2Idle.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player2Idle.getSpriteWidth() / 2 + 300, 0, &clipRect3);
		DrawMainMenu(renderer);
	}
	else if (inOnePlayerSubMenu) {
		player1Quizical.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player1Appear.getSpriteWidth() / 2, 0, &clipRect);
		player2Idle.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player2Idle.getSpriteWidth() / 2 + 300, 0, &clipRect3);
		player1Select.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player1Select.getSpriteWidth() / 2, 0, &clipRect);
		if (!player1Select.isPlaying())
			DrawOnePlayerSubMenu(renderer);
	}
	else if (inTwoPlayerSubMenu) {
		player1Quizical.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player1Appear.getSpriteWidth() / 2, 0, &clipRect);
		player2Quizical.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player2Idle.getSpriteWidth() / 2 + 300, 0, &clipRect3);
		player1Select.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player1Select.getSpriteWidth() / 2, 0, &clipRect);
		player2Select.render(renderer, CURRENT_SCREEN_WIDTH / 2 - player2Select.getSpriteWidth() / 2 + 300, 0, &clipRect3);
		if (!player2Select.isPlaying())
			DrawTwoPlayerSubMenu(renderer);
	}
	else if (inGame) {
		if (beginGame) {
			gameWorld.render(renderer);
			for (int i = 0; i < pickupGathering.size(); i++) {
				pickupGathering[i]->render(renderer);
			}
			if (twoPlayerMode)
				rival.render(renderer);
			snake.render(renderer);
			tb->render(renderer, 0, 120, NULL);
			score->render(renderer, CURRENT_SCREEN_WIDTH - 250, 0);
			player1SpeedText->render(renderer, 0, 0);
			if (twoPlayerMode)
				player2SpeedText->render(renderer, 0, 40);
			pickupGenerationSpeedText->render(renderer, 0, 80);
			if (endGameState)
				winLoseText->render(renderer, CURRENT_SCREEN_WIDTH / 2, CURRENT_SCREEN_HEIGHT / 2);
		}
		else {
			if (!twoPlayerMode) {
				player1FallingAnimation(&snake);
			}
			else {
				player1FallingAnimation(&snake);
				player2FallingAnimation(&rival);
			}
			gameWorld.render(renderer);
		}
	}
	if (menuIsOpen) {
		inGameMenu->render(renderer, {255,255,255});
	}
	
	SDL_RenderPresent(renderer);
}

void Update(Uint32 deltaT) {
	static Uint32 accumulatedTimeForPlayer1 = 0;
	static Uint32 accumulatedTimeForPlayer2 = 0;

	player1Appear.update();
	player1Idle.update();
	player2Appear.update();
	player2Idle.update();
	player1Quizical.update();
	player2Quizical.update();
	player1Select.update();
	player2Select.update();
	//player1Unimpressed.update();
	//player2Angry.update();

	if (!menuIsOpen && inGame && beginGame) {
		accumulatedTimeForPlayer1 += deltaT;
		accumulatedTimeForPlayer2 += deltaT;
		snake.move(accumulatedTimeForPlayer1, &gameWorld);
		if (twoPlayerMode)
			rival.move(accumulatedTimeForPlayer2, &gameWorld);

		CollisionType player1CollisionType = NONE;
		player1CollisionType = snake.checkCollisions(&rival);
		if (twoPlayerMode) {
			CollisionType player2CollisionType = NONE;
			player2CollisionType = rival.checkCollisions(&snake);

			if (player1CollisionType == OTHER && player2CollisionType == OTHER) {	// check to see if both players ran into each other
				printf("Players ran into each! It's a draw!\n");
				endGameState = true;
			}
			else if (player1CollisionType == OTHER) {
				printf("%s ran into %s!\n", snake.getName().c_str(), rival.getName().c_str());
				endGameState = true;
			}
			else if (player2CollisionType == OTHER) {
				printf("%s ran into %s!\n", rival.getName().c_str(), snake.getName().c_str());
				endGameState = true;
			}
			if (player2CollisionType == SELF) {
				printf("%s ran into themselves!\n", rival.getName().c_str());
				endGameState = true;
			}
			if (player2CollisionType == WALL) {
				printf("%s ran into a wall!\n", rival.getName().c_str());
				endGameState = true;
			}
		}

		if (player1CollisionType == SELF) {
			printf("%s ran into themselves!\n", snake.getName().c_str());
			endGameState = true;
		}
		if (player1CollisionType == WALL) {
			printf("%s ran into a wall!\n", snake.getName().c_str());
			endGameState = true;
		}

		Uint32 genSpeed = gameWorld.generatePickups(deltaT);
		std::stringstream generationSpeedStr;
		generationSpeedStr.str("");
		generationSpeedStr << "Gen. Speed: " << genSpeed;
		pickupGenerationSpeedText->loadFromRenderedText(renderer, generationSpeedStr.str().c_str(), DefaultFont, { 255,255,255 });
	}
}

void addHeadNodeAndConnectionsToGraph(Graph* graph,Snake* snake, GameWorld* gameWorld) {
	// get the id of the last node in graph
	std::vector<Node>* nodesInGraph = graph->getNodes();
	Node lastNodeInGraph = nodesInGraph->at(nodesInGraph->size() - 1);
	int nodeId = lastNodeInGraph.getID();

	Cell* snakeHeadCell = gameWorld->getCell(snake->getGridPosition().row, snake->getGridPosition().column);
	Node snakeHeadNode = Node(snakeHeadCell->getGridPosition());
	snakeHeadNode.setID(++nodeId);
	graph->addNode(snakeHeadNode);

	GridPosition thisCellsGP = snakeHeadCell->getGridPosition();

	// add the rightward connection
	if (thisCellsGP.column < NUMOFCOLS - 1) {
		Cell* cellToTheRight = gameWorld->getCell(thisCellsGP.row, thisCellsGP.column + 1); // NOTE: nodes on right column will not have rightward connections, need to fix this; can fix by looking at grid position?
		bool cellOnRightOccupied = cellToTheRight->getOccupied();
		if (!cellOnRightOccupied) {
			Node nodeOnRight = Node(cellToTheRight->getGridPosition());
			Connection newConnectionRight = Connection(snakeHeadNode, nodeOnRight);
			graph->addConnection(newConnectionRight);
		}
	}
	// add the netherward connection
	if (thisCellsGP.row < NUMOFROWS - 1) {
		Cell* cellBelow = gameWorld->getCell(thisCellsGP.row + 1, thisCellsGP.column); // NOTE: nodes on bottom row will not have netherward connections, need to fix this
		bool cellBelowOccupied = cellBelow->getOccupied();
		if (!cellBelowOccupied) {
			Node nodeBelow = Node(cellBelow->getGridPosition());
			Connection newConnectionBelow = Connection(snakeHeadNode, nodeBelow);
			graph->addConnection(newConnectionBelow);
		}
	}
	// add the leftward connection
	if (thisCellsGP.column > 0) {
		Cell* cellLeft = gameWorld->getCell(thisCellsGP.row, thisCellsGP.column - 1); // NOTE: nodes on bottom row will not have netherward connections, need to fix this
		bool cellLeftOccupied = cellLeft->getOccupied();
		if (!cellLeftOccupied) {
			Node nodeBelow = Node(cellLeft->getGridPosition());
			Connection newConnectionLeft = Connection(snakeHeadNode, nodeBelow);
			graph->addConnection(newConnectionLeft);
		}
	}
	// add the upward connection
	if (thisCellsGP.row > 0) {
		Cell* cellAbove = gameWorld->getCell(thisCellsGP.row - 1, thisCellsGP.column); // NOTE: nodes on bottom row will not have netherward connections, need to fix this
		bool cellAboveOccupied = cellAbove->getOccupied();
		if (!cellAboveOccupied) {
			Node nodeAbove = Node(cellAbove->getGridPosition());
			Connection newConnectionAbove = Connection(snakeHeadNode, nodeAbove);
			graph->addConnection(newConnectionAbove);
		}
	}
}

int main(int argc, char* args[]) {
	if (!init()) {
		printf("Failed to initialize app!\n");
	}
	else {
		if (!loadMedia()) {
			printf("Failed to load media!\n");
		}
		else {

			//if (Mix_PlayingMusic() == 0) {
			//	Mix_PlayMusic(music, -1);
			//}

			bool pauseGame = false;

			SDL_Event e;

			snake.setSnakeTexture(&player1Sheet);
			rival.setSnakeTexture(&player2Sheet);

			Uint32 startTime = SDL_GetTicks();
			Uint32 endTime = 0;
			Uint32 deltaT = 0;

			Timer timeSinceLastFrame = Timer();
			timeSinceLastFrame.start();

			Uint32 totalMS = 0;
			int countedFrames = 0;
			totalMS = SDL_GetTicks();

			std::stringstream player1SpeedStr;
			std::stringstream player2SpeedStr;
			std::stringstream scoreStr;
			std::stringstream fpsStr;

			scoreStr.str("Score: 0");

			winLoseText->loadFromRenderedText(renderer, "The game is over!", DefaultFont, { 255,255,255 });
			score->loadFromRenderedText(renderer, scoreStr.str().c_str(), DefaultFont, {255,255,255});

			startMenu = new Menu(CURRENT_SCREEN_WIDTH / 2 - mainMenuTexture.getWidth() / 2, 0, CURRENT_SCREEN_WIDTH, CURRENT_SCREEN_HEIGHT);
			startMenu->setTexture(&mainMenuTexture);

			mainMenu = new Menu(CURRENT_SCREEN_WIDTH * 11 / 40, CURRENT_SCREEN_HEIGHT * 7 / 10, CURRENT_SCREEN_WIDTH * 9 / 20, CURRENT_SCREEN_HEIGHT / 4);
			mainMenu->setTextDimensions(0.5f, 0.15f);
			mainMenu->setTextPosition(0.1f, 0.5f);
			SDL_Point mainMenuButtonPos = mainMenu->getPos();
			mainMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2 ;
			mainMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5;
			Button* newButton = new Button("One Player", mainMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "One Player", pixelFont, { 255,255,255 });
			int buttonTextWidth = newButton->getLabel1()->getWidth();
			mainMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(mainMenuButtonPos);
			newButton->setBehavior(OnePlayerSetup);
			mainMenu->addButton(*newButton);
			delete newButton;
			mainMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			mainMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5 + 95;
			newButton = new Button("Two Player", mainMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Two Player", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			mainMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(mainMenuButtonPos);
			newButton->setBehavior(TwoPlayerSetup);
			mainMenu->addButton(*newButton);
			delete newButton;
			mainMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			mainMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5 + 190;
			newButton = new Button("Quit Game", mainMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Quit Game", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			mainMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(mainMenuButtonPos);
			newButton->setBehavior(QuitGame);
			mainMenu->addButton(*newButton);
			delete newButton;

			onePlayerSubMenu = new Menu(CURRENT_SCREEN_WIDTH * 11 / 40, CURRENT_SCREEN_HEIGHT * 7 / 10, CURRENT_SCREEN_WIDTH * 9 / 20, CURRENT_SCREEN_HEIGHT / 4);
			onePlayerSubMenu->setTextDimensions(0.5f, 0.15f);
			onePlayerSubMenu->setTextPosition(0.1f, 0.5f);
			SDL_Point onePlayerSubMenuButtonPos = onePlayerSubMenu->getPos();
			onePlayerSubMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			onePlayerSubMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5;
			newButton = new Button("Play One Player", onePlayerSubMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Play One Player", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			onePlayerSubMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(onePlayerSubMenuButtonPos);
			newButton->setBehavior(OnePlayerStartHuman);
			onePlayerSubMenu->addButton(*newButton);
			delete newButton;
			onePlayerSubMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			onePlayerSubMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5 + 95;
			newButton = new Button("Let AI Play", onePlayerSubMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Let AI Play", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			onePlayerSubMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(onePlayerSubMenuButtonPos);
			newButton->setBehavior(OnePlayerStartAI);
			onePlayerSubMenu->addButton(*newButton);
			delete newButton;
			onePlayerSubMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			onePlayerSubMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5 + 190;
			newButton = new Button("Back", onePlayerSubMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Back", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			onePlayerSubMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(onePlayerSubMenuButtonPos);
			newButton->setBehavior(BackToMainMenu);
			onePlayerSubMenu->addButton(*newButton);
			delete newButton;

			twoPlayerSubMenu = new Menu(CURRENT_SCREEN_WIDTH * 11 / 40, CURRENT_SCREEN_HEIGHT * 7 / 10, CURRENT_SCREEN_WIDTH * 9 / 20, CURRENT_SCREEN_HEIGHT / 4);
			twoPlayerSubMenu->setTextDimensions(0.5f, 0.15f);
			twoPlayerSubMenu->setTextPosition(0.1f, 0.5f);
			SDL_Point twoPlayerSubMenuButtonPos = twoPlayerSubMenu->getPos();
			twoPlayerSubMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			twoPlayerSubMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5;
			newButton = new Button("Play as Two Human Players", twoPlayerSubMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Play as Two Human Players", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			twoPlayerSubMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(twoPlayerSubMenuButtonPos);
			newButton->setBehavior(TwoPlayerStartHuman);
			twoPlayerSubMenu->addButton(*newButton);
			delete newButton;
			twoPlayerSubMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			twoPlayerSubMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5 + 95;
			newButton = new Button("Play as One Human and One AI", twoPlayerSubMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Play as One Human and One AI", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			twoPlayerSubMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(twoPlayerSubMenuButtonPos);
			newButton->setBehavior(TwoPlayerStartOneAI);
			twoPlayerSubMenu->addButton(*newButton);
			delete newButton;
			twoPlayerSubMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			twoPlayerSubMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5 + 190;
			newButton = new Button("Watch Two AI Play", twoPlayerSubMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Watch Two AI Play", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			twoPlayerSubMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(twoPlayerSubMenuButtonPos);
			newButton->setBehavior(TwoPlayerStartTwoAI);
			twoPlayerSubMenu->addButton(*newButton);
			delete newButton;
			twoPlayerSubMenuButtonPos.x = CURRENT_SCREEN_WIDTH / 2;
			twoPlayerSubMenuButtonPos.y = CURRENT_SCREEN_HEIGHT * 3 / 5 + 285;
			newButton = new Button("Back", twoPlayerSubMenuButtonPos, 200, 50);
			newButton->setLabel1(renderer, "Back", pixelFont, { 255,255,255 });
			buttonTextWidth = newButton->getLabel1()->getWidth();
			twoPlayerSubMenuButtonPos.x -= buttonTextWidth / 2;
			newButton->setPos(twoPlayerSubMenuButtonPos);
			newButton->setBehavior(BackToMainMenu);
			twoPlayerSubMenu->addButton(*newButton);
			delete newButton;

			inGameMenu = new Menu(CURRENT_SCREEN_WIDTH / 2 - 250, CURRENT_SCREEN_HEIGHT / 2 - 250, 500, 500, "");
			inGameMenu->setTextDimensions(0.7f, 0.12f);
			inGameMenu->setTextPosition(0.15f, 0.1f);

			SDL_Point menuPos = inGameMenu->getPos();
			menuPos.x += 200;
			menuPos.y += 200;
			Button* button = new Button("Restart", menuPos, 100, 50);
			button->setBehavior(restartGame);
			inGameMenu->addButton(*button);

			menuPos.x += 8;
			menuPos.y += 100;
			button = new Button("Quit", menuPos, 80, 50);
			button->setBehavior(QuitGame);
			inGameMenu->addButton(*button);

			GraphGenerator graphGen = GraphGenerator();

			player1Appear.setSpriteDimensions(1000, 1000);
			player1Appear.setAnimationTime(1200);
			player1Appear.setNumOfFrames(7);

			player1Idle.setSpriteDimensions(1000, 1000);
			player1Idle.setAnimationTime(1800);
			player1Idle.setNumOfFrames(13);
			player1Idle.setLooping(true);

			player2Appear.setSpriteDimensions(1000, 1000);
			player2Appear.setAnimationTime(1500);
			player2Appear.setNumOfFrames(8);

			player2Idle.setSpriteDimensions(1000, 1000);
			player2Idle.setAnimationTime(1800);
			player2Idle.setNumOfFrames(15);
			player2Idle.setLooping(true);

			player1Quizical.setSpriteDimensions(1000, 1000);
			player1Quizical.setAnimationTime(1000);
			player1Quizical.setNumOfFrames(4);
			player1Quizical.setLooping(true);

			player1Select.setSpriteDimensions(1000, 1000);
			player1Select.setAnimationTime(1500);
			player1Select.setNumOfFrames(8);

			player2Select.setSpriteDimensions(1000, 1000);
			player2Select.setAnimationTime(2000);
			player2Select.setNumOfFrames(11);

			//player1Unimpressed.setSpriteDimensions(1000, 1000);
			//player1Unimpressed.setAnimationTime(1500);
			//player1Unimpressed.setNumOfFrames(8);
			//player1Unimpressed.startAnimation();

			player2Quizical.setSpriteDimensions(1000, 1000);
			player2Quizical.setAnimationTime(1000);
			player2Quizical.setNumOfFrames(5);
			player2Quizical.setLooping(true);

			//player2Angry.setSpriteDimensions(1000, 1000);
			//player2Angry.setAnimationTime(1500);
			//player2Angry.setNumOfFrames(8);
			//player2Angry.startAnimation();

			startMenuText->setFlashingText(true);
			startMenuText->loadFromRenderedText(renderer, "Press Any Key", pixelFont, { 255,255,255 });

			while (runGame) {
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT) {
						runGame = false;
					}
					else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
						menuIsOpen = !menuIsOpen;
						break;
					}
					else if (menuIsOpen) {
						if (e.type == SDL_MOUSEMOTION) 
							inGameMenu->handleEvent(&e);
						else if (e.type == SDL_MOUSEBUTTONDOWN) 
							inGameMenu->handleEvent(&e);
					}
					else if (inStartMenu && e.type == SDL_KEYDOWN) {
						inStartMenu = false;
						inMainMenu = true;
					}
					else if (inMainMenu) {
						if (e.type == SDL_MOUSEMOTION)
							mainMenu->handleEvent(&e);
						else if (e.type == SDL_MOUSEBUTTONDOWN)
							mainMenu->handleEvent(&e);
					}
					else if (inOnePlayerSubMenu) {
						if (e.type == SDL_MOUSEMOTION)
							onePlayerSubMenu->handleEvent(&e);
						else if (e.type == SDL_MOUSEBUTTONDOWN)
							onePlayerSubMenu->handleEvent(&e);
					}
					else if (inTwoPlayerSubMenu) {
						if (e.type == SDL_MOUSEMOTION)
							twoPlayerSubMenu->handleEvent(&e);
						else if (e.type == SDL_MOUSEBUTTONDOWN)
							twoPlayerSubMenu->handleEvent(&e);
					}
					else if (inGame && e.type == SDL_KEYDOWN && !menuIsOpen && !aiControlsPlayer1) {
						switch (e.key.keysym.sym) {
						case SDLK_a:
							if (snake.getDirection() != RIGHT) {
								snake.setDesiredDirection(LEFT);
							}
							break;
						
						case SDLK_d:
							if (snake.getDirection() != LEFT) {
								snake.setDesiredDirection(RIGHT);
							}
							break;
						
						case SDLK_w:
							if (snake.getDirection() != DOWN) {
								snake.setDesiredDirection(UP);
							}
							break;

						case SDLK_s:
							if (snake.getDirection() != UP) {
								snake.setDesiredDirection(DOWN);
							}
							break;
						
							if (twoPlayerMode && !aiControlsPlayer2) {
								case SDLK_LEFT:
									if (rival.getDirection() != RIGHT) {
										rival.setDesiredDirection(LEFT);
									}
									break;

								case SDLK_RIGHT:
									if (rival.getDirection() != LEFT) {
										rival.setDesiredDirection(RIGHT);
									}
									break;

								case SDLK_UP:
									if (rival.getDirection() != DOWN) {
										rival.setDesiredDirection(UP);
									}
									break;

								case SDLK_DOWN:
									if (rival.getDirection() != UP) {
										rival.setDesiredDirection(DOWN);
									}
									break;
							}

						case SDLK_p:
							pauseGame = !pauseGame;
						default:
							break;
						}
					}

					window->handleEvent(e, renderer);
					if (windowResized) {
						windowResized = false;

						UNIT_DISTANCE = (window->getHeight() - paddingY) / NUMOFCOLS;
						
						CURRENT_SCREEN_HEIGHT = window->getHeight();
						CURRENT_SCREEN_WIDTH = window->getWidth();
						
						gameWorld.resizeGameWorld(UNIT_DISTANCE);
					}
				}

				if (!endGameState && (aiControlsPlayer1 || aiControlsPlayer2)) {
					Graph graph = graphGen.convertWorldToGraph(gameWorld);
					//player1SpeedStr.str("");
					//player1SpeedStr << "Size of Graph: " << graph.getNumOfNodes();
					//player1SpeedText->loadFromRenderedText(renderer, player1SpeedStr.str().c_str(), DefaultFont, { 255,255,255 });

					if (aiControlsPlayer1) {
						addHeadNodeAndConnectionsToGraph(&graph, &snake, &gameWorld);

						Direction player1Direction = ai.getDecision(&graph, &snake, &pickupGathering);
						snake.setDesiredDirection(player1Direction);
					}
					
					if (twoPlayerMode && aiControlsPlayer2) {
						addHeadNodeAndConnectionsToGraph(&graph, &rival, &gameWorld);

						Direction player2Direction = ai.getDecision(&graph, &rival, &pickupGathering);
						rival.setDesiredDirection(player2Direction);
					}
				}

				player1SpeedStr.str("");
				uint player1Speed = snake.getSpeed();
				player1SpeedStr << "Player 1 Speed: " << player1Speed;
				player1SpeedText->loadFromRenderedText(renderer, player1SpeedStr.str().c_str(), DefaultFont, { 255,255,255 });

				if (twoPlayerMode) {
					player2SpeedStr.str("");
					uint player2Speed = rival.getSpeed();
					player2SpeedStr << "Player 2 Speed: " << player2Speed;
					player2SpeedText->loadFromRenderedText(renderer, player2SpeedStr.str().c_str(), DefaultFont, { 255,255,255 });
				}

				totalMS = SDL_GetTicks();
				float avgFPS = countedFrames / (totalMS / 1000.0f);
				avgFPS = floor(avgFPS);
				if (avgFPS > 20000)
					avgFPS = 0;

				fpsStr.str("");
				fpsStr << "avgFPS: " << avgFPS << "   deltaT: " << deltaT;
				tb->loadFromRenderedText(renderer, fpsStr.str().c_str(), DefaultFont, {255,255,255});

				//float framerate = 1000.0f / deltaT;
				
				deltaT = timeSinceLastFrame.getTicks();
				timeSinceLastFrame.start();

				scoreStr.str("");
				scoreStr << "Score: " << gameScore;
				score->loadFromRenderedText(renderer, scoreStr.str().c_str(), DefaultFont, { 255,255,255 });


				if (!endGameState && !pauseGame) {
					Update(deltaT);
				}

				if (inMainMenu) {
					if (!player1Appear.isPlaying() && !player1Appear.isFinished()) {
						player1Appear.startAnimation();
						player2Appear.startAnimation();
					}
					if (player1Appear.isFinished() && !player1Idle.isPlaying()) {
						player1Idle.startAnimation();
					}
					if (player2Appear.isFinished() && !player2Idle.isPlaying()) {
						player2Idle.startAnimation();
					}
				}

				if (inOnePlayerSubMenu && player1Select.isFinished()) {
					inOnePlayerSubMenu = false;
					inGame = true;
				}

				if (inTwoPlayerSubMenu && player2Select.isFinished()) {
					inTwoPlayerSubMenu = false;
					twoPlayerMode = true;
					inGame = true;
				}

				if (!window->isMinimized()) {
					Render(renderer);
				}

				++countedFrames;

				if (endGameState) {

				}

				startMenuText->loadFromRenderedText(renderer, "Press Any Key", pixelFont, startMenuText->getTextColor());
			}

			close();
		}
	}

	return 0;
}