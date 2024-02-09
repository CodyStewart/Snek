#include "texture.h"

Texture::Texture() {
	texture = nullptr;
	width = 0;
	height = 0;
}

Texture::~Texture() {
	free();
}

SDL_Texture* Texture::loadTextureFromFile(SDL_Renderer* renderer, std::string path) {
	texture = IMG_LoadTexture(renderer, path.c_str());
}

void Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip) {
	SDL_Rect renderQuad = { x, y, width, height };

	if (clip != NULL) {
		renderQuad.w = clip->w;
		renderQuad.h = clip->h;
	}

	SDL_RenderCopy(renderer, texture, clip, &renderQuad);
}

void Texture::free() {
	if (texture != NULL)
	{
		SDL_DestroyTexture(texture);
		texture = NULL;
		width = 0;
		height = 0;
	}
}

int Texture::getWidth() { return width; }
int Texture::getHeight() { return height; }