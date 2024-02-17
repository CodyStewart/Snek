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

bool aiControlsPlayer1 = true;
bool aiControlsPlayer2 = false;
bool twoPlayerMode = false;
bool windowResized = false;
bool runGame = true;
bool endGameState = false;
bool menuIsOpen = false;
uint gameScore = 0;

GameWorld gameWorld = GameWorld(UNIT_DISTANCE, NUMOFROWS, NUMOFCOLS);
Snake snake = Snake(&gameWorld, player1, "Player 1");
Snake rival = Snake(&gameWorld, player2, "Player 2");
AI ai = AI();


std::vector<PickUp*> pickupGathering;

TTF_Font* DefaultFont = NULL;

TextTexture* tb = new TextTexture();
TextTexture* winLoseText = new TextTexture();
TextTexture* score = new TextTexture();
TextTexture* player1SpeedText = new TextTexture();
TextTexture* player2SpeedText = new TextTexture();
TextTexture* pickupGenerationSpeedText = new TextTexture();

Menu* menu;

Mix_Music* music;
Mix_Chunk* player1Sound;
Mix_Chunk* player2Sound;

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

	// load music
	music = Mix_LoadMUS("Apoxode_-_Oortian_Clouds_1.mp3");
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
	if (menuIsOpen) {
		menu->render(renderer);
	}
	SDL_RenderPresent(renderer);
}

void Update(Uint32 deltaT) {
	static Uint32 accumulatedTimeForPlayer1 = 0;
	static Uint32 accumulatedTimeForPlayer2 = 0;

	if (!menuIsOpen) {
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

			menu = new Menu(CURRENT_SCREEN_WIDTH / 2 - 250, CURRENT_SCREEN_HEIGHT / 2 - 250, 500, 500, "");
			menu->setTextDimensions(0.7f, 0.12f);
			menu->setTextPosition(0.15f, 0.1f);

			SDL_Point menuPos = menu->getPos();
			menuPos.x += 200;
			menuPos.y += 200;
			Button* button = new Button("Restart", menuPos, 100, 50);
			button->setBehavior(restartGame);
			menu->addButton(*button);

			menuPos.x += 8;
			menuPos.y += 100;
			button = new Button("Quit", menuPos, 80, 50);
			button->setBehavior(QuitGame);
			menu->addButton(*button);

			GraphGenerator graphGen = GraphGenerator();

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
							menu->handleEvent(&e);
						else if (e.type == SDL_MOUSEBUTTONDOWN) 
							menu->handleEvent(&e);

					}
					else if (e.type == SDL_KEYDOWN && !menuIsOpen && !aiControlsPlayer1) {
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

				if (!window->isMinimized()) {
					Render(renderer);
				}

				++countedFrames;

				if (endGameState) {

				}
			}

			close();
		}
	}

	return 0;
}