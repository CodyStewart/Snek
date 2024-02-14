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

bool windowResized = false;
bool runGame = true;
bool endGameState = false;
bool menuIsOpen = false;
uint gameScore = 0;

GameWorld gameWorld = GameWorld(UNIT_DISTANCE, NUMOFROWS, NUMOFCOLS);
Snake snake = Snake(&gameWorld);
std::vector<PickUp*> pickupGathering;

TTF_Font* DefaultFont = NULL;

TextTexture* tb = new TextTexture();
TextTexture* winLoseText = new TextTexture();
TextTexture* score = new TextTexture();

Menu* menu;

Mix_Music* music;
Mix_Chunk* sound;

void restartGame() {
	gameScore = 0;
	endGameState = false;
	snake.getBody()->clear();
	snake.setBody(&gameWorld);
	snake.setDirection(UP);
	snake.setDesiredDirection(UP);
	snake.setSpeed(5);
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
	sound = Mix_LoadWAV("608645__theplax__crunch-5.wav");
	if (sound == NULL) {
		printf("Failed to load place token sound effect! SDL_mixer Error: %s\n", Mix_GetError());
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

	Mix_FreeChunk(sound);
	Mix_FreeMusic(music);
	sound = NULL;
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
	snake.render(renderer);
	tb->render(renderer, 0, 0, NULL);
	score->render(renderer, CURRENT_SCREEN_WIDTH - 250, 0);
	if (menuIsOpen) {
		menu->render(renderer);
	}
	if (endGameState)
		winLoseText->render(renderer, CURRENT_SCREEN_WIDTH / 2, CURRENT_SCREEN_HEIGHT / 2);
	SDL_RenderPresent(renderer);
}

void Update(Uint32 deltaT) {
	if (!menuIsOpen) {
		snake.move(deltaT, &gameWorld);
		if (snake.checkCollisions()) {
			endGameState = true;
		}
		gameWorld.generatePickups(deltaT);
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

			if (Mix_PlayingMusic() == 0) {
				Mix_PlayMusic(music, -1);
			}

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

			std::stringstream ss;
			std::stringstream scoreStr;
			scoreStr.str("Score: 0");

			winLoseText->loadFromRenderedText(renderer, "The game is over!", DefaultFont, { 255,255,255 });
			score->loadFromRenderedText(renderer, scoreStr.str().c_str(), DefaultFont, {255,255,255});

			menu = new Menu(CURRENT_SCREEN_WIDTH / 2 - 250, CURRENT_SCREEN_HEIGHT / 2 - 250, 500, 500, "");
			menu->setTextDimensions(0.7f, 0.12f);
			menu->setTextPosition(0.15f, 0.1f);

			SDL_Point menuPos = menu->getPos();
			menuPos.x += 200;
			menuPos.y += 200;
			Button* button = new Button("Retry", menuPos, 100, 50);
			button->setBehavior(restartGame);
			menu->addButton(*button);

			menuPos.x += 8;
			menuPos.y += 100;
			button = new Button("Quit", menuPos, 80, 50);
			button->setBehavior(QuitGame);
			menu->addButton(*button);

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
					else if (e.type == SDL_KEYDOWN && !menuIsOpen) {
						switch (e.key.keysym.sym) {
						case SDLK_LEFT:
						case SDLK_a:
							if (snake.getDirection() != RIGHT) {
								snake.setDesiredDirection(LEFT);
							}
							break;
						
						case SDLK_RIGHT:
						case SDLK_d:
							if (snake.getDirection() != LEFT) {
								snake.setDesiredDirection(RIGHT);
							}
							break;
						
						case SDLK_UP:
						case SDLK_w:
							if (snake.getDirection() != DOWN) {
								snake.setDesiredDirection(UP);
							}
							break;

						case SDLK_DOWN:
						case SDLK_s:
							if (snake.getDirection() != UP) {
								snake.setDesiredDirection(DOWN);
							}
							break;

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

				ss.str("");
				totalMS = SDL_GetTicks();
				float avgFPS = countedFrames / (totalMS / 1000.0f);
				avgFPS = floor(avgFPS);
				if (avgFPS > 20000)
					avgFPS = 0;

				deltaT = timeSinceLastFrame.getTicks();
				ss << "avgFPS: " << avgFPS << "   deltaT: " << deltaT;
				tb->loadFromRenderedText(renderer, ss.str().c_str(), DefaultFont, {255,255,255});
				timeSinceLastFrame.start();

				float framerate = 1000.0f / deltaT;

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