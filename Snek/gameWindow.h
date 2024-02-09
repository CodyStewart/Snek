#pragma once
#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include<SDL.h>

struct GameWindow
{
	GameWindow();

	bool init(int screenWidth, int screenHeight);

	SDL_Renderer* createRenderer();

	void handleEvent(SDL_Event& e, SDL_Renderer* renderer);

	void free();

	int getWidth();
	int getHeight();

	// window focii
	bool hasMouseFocus();
	bool hasKeyboardFocus();
	bool isMinimized();

private:
	SDL_Window* gWindow;

	int width;
	int height;

	bool mouseFocus;
	bool keyboardFocus;
	bool fullscreen;
	bool minimized;
};

#endif // !GAMEWINDOW_H
