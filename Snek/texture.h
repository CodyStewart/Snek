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

	void setFlashingText(bool value);
	void setFlashingPeriod(int period);

	SDL_Color getTextColor();
	int getWidth();
	int getHeight();
	std::string getText();
	TTF_Font* getFont();

	void setTextColor(SDL_Color color);
	
	bool loadFromRenderedText(SDL_Renderer* renderer, std::string text, TTF_Font* font, SDL_Color textColor);

	//void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip);

	void free();

private:
	std::string text;
	SDL_Color textColor;
	TTF_Font* font;

	bool isFlashing;
	Uint32 flashTime;
};

struct SpriteSheet : public Texture {
	SpriteSheet();

	bool loadSpriteSheet(SDL_Renderer* renderer, std::string path, int width, int height, int sprites);

	void render(SDL_Renderer* renderer, int spriteToRender, SDL_Rect* renderSurface, double angle = NULL, SDL_RendererFlip* flip = NULL, SDL_Rect* clip = NULL, SDL_Point* center = NULL);

private:
	int spriteWidth;
	int spriteHeight;
	int numOfSprites;
};

#endif // !TEXTURE_H
