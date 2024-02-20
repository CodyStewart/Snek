#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include<SDL.h>
#include<string>
#include<SDL_image.h>
#include<SDL_ttf.h>

struct Texture
{
	Texture();

	~Texture();

	bool loadTextureFromFile(SDL_Renderer* renderer, std::string path);
	
	void free();

	void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL);

	SDL_Texture* getTexture();
	int getWidth();
	int getHeight();

protected:
	SDL_Texture* texture;

	int width;
	int height;
};

struct TextTexture : public Texture {
	TextTexture();
	TextTexture(SDL_Renderer* renderer, std::string text, TTF_Font* font, SDL_Color color);

	~TextTexture();
	
	bool loadFromRenderedText(SDL_Renderer* renderer, std::string text, TTF_Font* font, SDL_Color textColor);

	//void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip);

	void free();

private:
	std::string text;
	SDL_Color textColor;
	TTF_Font* font;
};

#endif // !TEXTURE_H
