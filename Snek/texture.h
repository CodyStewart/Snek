#pragma once
#ifndef TEXTURE_H
#define TEXTURE_H

#include<SDL.h>
#include<string>
#include<SDL_image.h>

struct Texture
{
	Texture();

	~Texture();

	SDL_Texture* loadTextureFromFile(SDL_Renderer* renderer, std::string path);
	
	void free();

	void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL);

	int getWidth();
	int getHeight();

private:
	SDL_Texture* texture;

	int width;
	int height;
};

#endif // !TEXTURE_H
