#pragma once
#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <string>
#include <SDL.h>
#include <SDL_mixer.h>

#include "texture.h"
#include "timer.h"

struct Animation {
	Animation();
	Animation(Texture* spriteSheet);
	Animation(Texture* spriteSheet, Uint32 time);
	Animation(Texture* spriteSheet, int width, int height, int frames, Uint32 time);
	Animation(Texture* spriteSheet, int width, int height, int frames, Uint32 time, bool loopValue);

	void startAnimation();
	void stopAnimation();

	void setSprites(Texture* spriteSheet);
	//void addSprite(Texture* sprite);
	void setAnimationTime(int time);
	void setSpriteDimensions(int width, int height);
	void setNumOfFrames(int frames);
	void setLooping(bool loopValue);
	void setSoundEffect(Mix_Chunk* soundEff);

	int getSpriteWidth();
	int getSpriteHeight();
	int getNumOfFrames();
	int getTiming();
	bool isPlaying();
	bool isFinished();

	bool loadAnimationFromFile(SDL_Renderer* renderer, std::string path);

	void playSoundEffect();

	void update();
	//void render(SDL_Renderer* renderer, int x, int y);
	void render(SDL_Renderer* renderer, int x, int y, SDL_Rect* clip = NULL);

private:
	Texture* sprites;

	Mix_Chunk* soundEffect;

	Timer timer;

	int frameCounter;

	int spriteWidth;
	int spriteHeight;
	int numOfFrames;
	Uint32 timePerFrame;
	Uint32 totalAnimationTime;

	bool loop;
};

#endif // !ANIMATION_H
