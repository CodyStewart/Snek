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

SDL_Texture* Texture::getTexture() { return texture; }
int Texture::getWidth() { return width; }
int Texture::getHeight() { return height; }

TextTexture::TextTexture() {
	texture = nullptr;
	font = nullptr;
	textColor = {};
	text = "";
	width = 0;
	height = 0;
	
	isFlashing = false;
	flashTime = 0;
}

TextTexture::TextTexture(SDL_Renderer* renderer, std::string tex, TTF_Font* font, SDL_Color color) {
	text = tex;
	font = font;
	textColor = color;

	loadFromRenderedText(renderer, text, font, textColor);
	
	isFlashing = false;
	flashTime = 0;
}

TextTexture::~TextTexture() {

}

void TextTexture::setFlashingText(bool value) {
	isFlashing = value;
}

void TextTexture::setFlashingPeriod(int period) {
	flashTime = (Uint32)period;
}

void TextTexture::setTextColor(SDL_Color color) {
	textColor = color;
}

SDL_Color TextTexture::getTextColor() {
	return textColor;
}

int TextTexture::getWidth() { return width; }
int TextTexture::getHeight() { return height; }
std::string TextTexture::getText() { return text; }
TTF_Font* TextTexture::getFont() { return font; }

bool TextTexture::loadFromRenderedText(SDL_Renderer* renderer, std::string tex, TTF_Font* fon, SDL_Color color) {
	free();

	text = tex;
	textColor = color;
	font = fon;

	if (isFlashing) {
		static int flashValue = textColor.a;
		if (flashValue == 0) {
			flashValue = 255;
			textColor.a = flashValue;
		}
		else if (flashValue > 2 && flashValue <= 255) {
			flashValue -= 2;
			textColor.a = flashValue;
		}
		else if (flashValue < 2 && flashValue > -255) {
			flashValue -= 2;
			textColor.a = -1 * flashValue;
		}
		else {
			flashValue *= -1;
			flashValue -= 2;
			textColor.a = flashValue;
		}
	}

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

SpriteSheet::SpriteSheet() {
	texture = nullptr;
	width = 0;
	height = 0;
	spriteWidth = 0;
	spriteHeight = 0;
	numOfSprites = 0;
}

bool SpriteSheet::loadSpriteSheet(SDL_Renderer* renderer, std::string path, int width, int height, int sprites) {
	if (Texture::loadTextureFromFile(renderer, path)) {
		spriteWidth = width;
		spriteHeight = height;
		numOfSprites = sprites;
		
		return true;
	}
	else
		return false;
}

void SpriteSheet::render(SDL_Renderer* renderer, int spriteToRender, SDL_Rect* renderSurface, double angle, SDL_RendererFlip* flip, SDL_Rect* clip, SDL_Point* center) {
	if (texture != NULL) {
		SDL_Rect sprite = {};
		int spriteXPos = spriteToRender * spriteWidth;
		sprite.x = spriteXPos;
		sprite.y = 0;
		sprite.w = spriteWidth;
		sprite.h = spriteHeight;

		if (clip != NULL) {
			sprite.x += clip->x;
			sprite.y = clip->y;
			sprite.w = clip->w;
			sprite.h = clip->h;
		}

		double rotation;
		if (angle == NULL)
			rotation = 0;
		else
			rotation = angle;

		SDL_RendererFlip flipMode;
		if (flip == NULL) {
			flipMode = SDL_FLIP_NONE;
		}
		else {
			flipMode = *flip;
		}

		SDL_RenderCopyEx(renderer, texture, &sprite, renderSurface, rotation, center, flipMode);
	}
}