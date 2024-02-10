#include "texture.h"

Texture::Texture() {
	texture = nullptr;
	width = 0;
	height = 0;
}

Texture::~Texture() {
	free();
}

bool Texture::loadTextureFromFile(SDL_Renderer* renderer, std::string path) {
	texture = IMG_LoadTexture(renderer, path.c_str());
	if (texture != NULL) {
		SDL_QueryTexture(texture, NULL, NULL, &width, &height);
		return true;
	}
	else {
		return false;
	}
}

void Texture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip) {
	if (texture != NULL) {
		SDL_Rect renderQuad = { x, y, width, height };

		if (clip != NULL) {
			renderQuad.w = clip->w;
			renderQuad.h = clip->h;
		}

		SDL_RenderCopy(renderer, texture, clip, &renderQuad);
	}
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

TextTexture::TextTexture() {
	texture = nullptr;
	font = nullptr;
	textColor = {};
	text = "";
	width = 0;
	height = 0;
}

TextTexture::TextTexture(SDL_Renderer* renderer, std::string tex, TTF_Font* font, SDL_Color color) {
	text = tex;
	font = font;
	textColor = color;

	loadFromRenderedText(renderer, text, font, textColor);
}

TextTexture::~TextTexture() {

}

bool TextTexture::loadFromRenderedText(SDL_Renderer* renderer, std::string tex, TTF_Font* fon, SDL_Color color) {
	free();

	text = tex;
	textColor = color;
	font = fon;

	SDL_Surface* surface = TTF_RenderText_Blended(font, text.c_str(), textColor);
	if (surface == NULL) {
		printf("Was unable to render text surface! SDL_TTF ERROR: %s\n", TTF_GetError());
		return false;
	}
	else {
		texture = SDL_CreateTextureFromSurface(renderer, surface);

		if (texture == NULL) {
			printf("Was unable to create texture from rendered text! SDL_TTF ERROR: %s\n", TTF_GetError());
		}
		else {
			width = surface->w;
			height = surface->h;
		}

		SDL_FreeSurface(surface);
		
		return true;
	}
}

//void TextTexture::render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip) {
//	if (texture != NULL) {
//		SDL_Rect renderQuad = { x, y, width, height };
//
//		if (clip != NULL) {
//			renderQuad.w = clip->w;
//			renderQuad.h = clip->h;
//		}
//
//		SDL_RenderCopy(renderer, texture, clip, &renderQuad);
//	}
//}

void TextTexture::free() {
	if (texture != nullptr)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
		font = nullptr;
		width = 0;
		height = 0;
	}
}