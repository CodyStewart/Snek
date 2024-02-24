#pragma once
#ifndef MENU_H
#define MENU_H

#include <string>
#include <vector>
#include <SDL.h>
#include <SDL_ttf.h>

#include "gameWorld.h"

extern GameWorld gameWorld;
extern TTF_Font* DefaultFont;
extern SDL_Renderer* renderer;

class TextBox {
public:
	TextBox();
	TextBox(std::string text);
	TextBox(std::string text, TTF_Font* font, SDL_Color color);
	TextBox(std::string text, TTF_Font* font, int xPos, int yPos, int width, int height, SDL_Color color);
	~TextBox();

	SDL_Color getTextColor();
	SDL_Rect* getRect();
	std::string getText();
	SDL_Texture* getTexture();

	void resetTexture();
	void setTextColor(SDL_Color color);
	void setPosition(int x, int y);
	void setDimensions(int w, int h);
	void setText(std::string text, TTF_Font* font);
	void render(SDL_Renderer* renderer);

private:
	std::string text;
	SDL_Color textColor;
	TTF_Font* font = NULL;
	SDL_Rect* box = NULL;
	SDL_Texture* texture = NULL;

	void setTexture(TTF_Font* font);
};

class Button {
public:
	Button();
	Button(std::string buttonText, SDL_Point pos, int w, int h);

	TextBox* getLabel();
	TextTexture* getLabel1();
	SDL_Rect getRect();
	SDL_Point getPosition();
	int getID();

	void setHighlighted(bool value);
	void setID(int id);
	void setBehavior(void(*func)());
	void setLabel1(SDL_Renderer* renderer, std::string text, TTF_Font* font, SDL_Color textColor);
	void setLabel1Color(SDL_Color textColor);
	void setPos(SDL_Point pos);

	void handleEvent();

	bool IsHighlighted();

private:
	void (*behavior)(void);

	SDL_Point position;
	TextBox* label;
	TextTexture* label1;
	SDL_Rect box;
	int id = -1;
	int width;
	int height;
	bool highlighted = false;
};

class Menu {
public:
	Menu();
	Menu(int x, int y, int width, int height, std::string text = "");

	void addButton(Button button);
	SDL_Rect* getRect();
	SDL_Point getPos();
	Button* getButton(int index);

	void setText(std::string text);
	void setText(std::string text, TTF_Font* font);
	void setTextPosition(float x, float y);
	void setTextDimensions(float w, float h);
	void setTexture(Texture* menuTexture);
	void resetTexture();

	void handleEvent(SDL_Event* e);
	void render(SDL_Renderer* renderer, SDL_Color color, SDL_Rect* clip = NULL);

private:
	SDL_Point position;
	Texture* menuTexture;
	TextBox* textbox;
	SDL_Rect* outline;
	std::vector<Button> buttons;
	int width;
	int height;
};
#endif // !MENU_H