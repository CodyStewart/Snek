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

GameWorld gameWorld = GameWorld(UNIT_DISTANCE, NUMOFROWS, NUMOFCOLS);
Snake snake = Snake(&gameWorld);

TTF_Font* DefaultFont = NULL;

TextTexture* tb = new TextTexture();

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

	//// load music
	//music = Mix_LoadMUS("ukulele.mp3");
	//if (music == NULL) {
	//	printf("Failed to load music! SDL_mixer Error: %s\n", Mix_GetError());
	//	//success = false;
	//}

	//// load sound effects
	//placeToken = Mix_LoadWAV("pop.wav");
	//if (placeToken == NULL) {
	//	printf("Failed to load place token sound effect! SDL_mixer Error: %s\n", Mix_GetError());
	//	//success = false;
	//}

	DefaultFont = TTF_OpenFont("consola.ttf", 36);
	if (DefaultFont == NULL) {
		printf("TTF could not open font! SDL_ttf Error: %s\n", TTF_GetError());
		success = false;
	}

	return success;
}

void close() {
	Mix_HaltMusic();

	//Mix_FreeChunk(placeToken);
	//Mix_FreeMusic(music);
	//placeToken = NULL;
	//music = NULL;

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
	snake.render(renderer);
	tb->render(renderer, 0, 0, NULL);
	SDL_RenderPresent(renderer);
}

void Update(Uint32 deltaT) {
	snake.move(deltaT);
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

			/*if (Mix_PlayingMusic() == 0) {
				Mix_PlayMusic(music, -1);
			}*/

			SDL_Event e;

			bool runGame = true;

			Uint32 startTime = SDL_GetTicks();
			Uint32 endTime = 0;
			Uint32 deltaT = 0;

			Timer timeSinceLastFrame = Timer();
			timeSinceLastFrame.start();

			Uint32 totalMS = 0;
			int countedFrames = 0;
			totalMS = SDL_GetTicks();

			std::stringstream ss;

			snake.setSpeed(5);

			while (runGame) {
				while (SDL_PollEvent(&e) != 0) {
					if (e.type == SDL_QUIT) {
						runGame = false;
					}
					else if (e.type == SDL_KEYDOWN)
					{
						switch (e.key.keysym.sym) {
						case SDLK_ESCAPE:
							runGame = false;
							break;

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

				Update(deltaT);

				if (!window->isMinimized()) {
					Render(renderer);
				}

				++countedFrames;
			}

			close();
		}
	}

	return 0;
}