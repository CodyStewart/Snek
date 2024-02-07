#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <SDL_ttf.h>
#include <stdio.h>
#include <string>

bool init();

bool loadMedia();

void close();

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

const int SCREEN_WIDTH = 1920;
const int SCREEN_HEIGHT = 1080;

TTF_Font* DefaultFont = NULL;

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

		window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (window == NULL) {
			printf("Window was not created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else {
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
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

	//DefaultFont = TTF_OpenFont("arial.ttf", 24);
	//if (DefaultFont == NULL) {
	//	printf("TTF could not open font! SDL_ttf Error: %s\n", TTF_GetError());
	//	success = false;
	//}

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

	SDL_DestroyWindow(window);
	window = NULL;

	Mix_Quit();
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

void Render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
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

						default:
							break;
						}
					}
				}

				Render(renderer);
			}
		}
	}

	return 0;
}