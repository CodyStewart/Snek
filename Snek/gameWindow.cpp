#include<stdio.h>
#include<sstream>

#include "gameWindow.h"

extern bool windowResized;

GameWindow::GameWindow() {
	gWindow = nullptr;
	mouseFocus = false;
	keyboardFocus = false;
	fullscreen = false;
	minimized = false;
	width = 0;
	height = 0;
}

bool GameWindow::init(int screenWidth, int screenHeight) {
	gWindow = SDL_CreateWindow("Snek", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screenWidth, screenHeight, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (gWindow != NULL) {
		mouseFocus = true;
		keyboardFocus = true;
		width = screenWidth;
		height = screenHeight;
	}

	return gWindow != NULL;
}

SDL_Renderer* GameWindow::createRenderer() {
	return SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
}

void GameWindow::handleEvent(SDL_Event& e, SDL_Renderer* renderer) {
	if (e.type == SDL_WINDOWEVENT) {
		bool updateCaption = false;

		switch (e.window.event) {
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			width = e.window.data1;
			height = e.window.data2;
			windowResized = true;
			SDL_RenderPresent(renderer);
			break;

		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(renderer);
			break;
		
		case SDL_WINDOWEVENT_ENTER:
			mouseFocus = true;
			updateCaption = true;
			break;

		case SDL_WINDOWEVENT_LEAVE:
			mouseFocus = false;
			updateCaption = true;
			break;

		case SDL_WINDOWEVENT_FOCUS_GAINED:
			keyboardFocus = true;
			updateCaption = true;
			break;

		case SDL_WINDOWEVENT_FOCUS_LOST:
			keyboardFocus = false;
			updateCaption = true;
			break;

		case SDL_WINDOWEVENT_MINIMIZED:
			minimized = true;
			break;

		case SDL_WINDOWEVENT_MAXIMIZED:
			minimized = false;
			break;

		case SDL_WINDOWEVENT_RESTORED:
			minimized = false;
			break;
		}

		if (updateCaption) {
			std::stringstream caption;
			caption << "Snek - MouseFocus: " << ((mouseFocus) ? "On" : "Off") << " KeyboardFocus: " << ((keyboardFocus) ? "On" : "Off");
			SDL_SetWindowTitle(gWindow, caption.str().c_str());
		}
	}
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_k) {
		if (fullscreen) {
			SDL_SetWindowFullscreen(gWindow, 0);
			fullscreen = false;
		}
		else {
			SDL_SetWindowFullscreen(gWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
			fullscreen = true;
			minimized = false;
		}
	}
}

void GameWindow::free() {
	SDL_DestroyWindow(gWindow);

	gWindow = nullptr;
	mouseFocus = false;
	keyboardFocus = false;
	fullscreen = false;
	minimized = false;
	width = 0;
	height = 0;
}

int GameWindow::getWidth() { return width; }
int GameWindow::getHeight() { return height; }

bool GameWindow::hasMouseFocus() { return mouseFocus; }
bool GameWindow::hasKeyboardFocus() { return keyboardFocus; }
bool GameWindow::isMinimized() { return minimized; }