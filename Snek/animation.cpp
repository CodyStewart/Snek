#include "animation.h"

Animation::Animation() {
	sprites = new Texture();
	spriteWidth = 0;
	spriteHeight = 0;
	numOfFrames = 0;
	totalAnimationTime = 0;
	frameCounter = 0;
	timePerFrame = 0;

	loop = false;
}

Animation::Animation(Texture* spriteSheet) {
	sprites = spriteSheet;
	spriteWidth = 0;
	spriteHeight = 0;
	numOfFrames = 0;
	totalAnimationTime = 0;
	frameCounter = 0;
	timePerFrame = 0;

	loop = false;
}

Animation::Animation(Texture* spriteSheet, Uint32 time) {
	sprites = spriteSheet;
	spriteWidth = 0;
	spriteHeight = 0;
	numOfFrames = 0;
	totalAnimationTime = time;
	frameCounter = 0;
	timePerFrame = 0;

	loop = false;
}

Animation::Animation(Texture* spriteSheet, int width, int height, int frames, Uint32 time) {
	sprites = spriteSheet;
	spriteWidth = width;
	spriteHeight = height;
	numOfFrames = frames;
	totalAnimationTime = time;
	frameCounter = 0;
	timePerFrame =  totalAnimationTime / (Uint32)frames;

	loop = false;
}

Animation::Animation(Texture* spriteSheet, int width, int height, int frames, Uint32 time, bool loopValue) {
	sprites = spriteSheet;
	spriteWidth = width;
	spriteHeight = height;
	numOfFrames = frames;
	totalAnimationTime = time;
	frameCounter = 0;
	timePerFrame = totalAnimationTime / (Uint32)frames;

	loop = loopValue;
}

void Animation::startAnimation() {
	timer.start();
}

void Animation::setSprites(Texture* spriteSheet) {
	if (sprites != nullptr)
		sprites = spriteSheet;
}

//void Animation::addSprite(Texture* sprite) {
//	sprites->push_back(*sprite);
//}

void Animation::setAnimationTime(int time) {
	totalAnimationTime = time;
}

void Animation::setSpriteDimensions(int width, int height) {
	spriteWidth = width;
	spriteHeight = height;
}

void Animation::setNumOfFrames(int frames) {
	numOfFrames = frames;
}

void Animation::setLooping(bool loopValue) {
	loop = loopValue;
}

int Animation::getSpriteWidth() { return spriteWidth; }
int Animation::getSpriteHeight() { return spriteHeight; }
int Animation::getNumOfFrames() { return numOfFrames; }
int Animation::getTiming() { return totalAnimationTime; }
bool Animation::isPlaying() { return timer.isStarted(); }

bool Animation::loadAnimationFromFile(SDL_Renderer* renderer, std::string path) {
	if (sprites->loadTextureFromFile(renderer, path.c_str())) {
		return true;
	}
	else {
		return false;
	}
}

void Animation::update() {
	timePerFrame = totalAnimationTime / (Uint32)numOfFrames;

	if (timer.isStarted()) {
		if (timer.getTicks() > timePerFrame) {
			if (frameCounter < numOfFrames - 1) {
				++frameCounter;
				timer.start();
			}
			else if (frameCounter == numOfFrames - 1 && loop) {
				frameCounter = 0;
				timer.start();
			}
			else {
				//frameCounter = 0;
				timer.stop();
			}
		}

	}
}

void Animation::render(SDL_Renderer* renderer, int x, int y) {
	if (timer.isStarted()) {
		int spriteXPos = frameCounter * spriteWidth;
		//int spriteYPos = frameCounter * spriteHeight;
		SDL_Rect spriteClip = { spriteXPos, 0, spriteWidth, spriteHeight };

		sprites->render(renderer, x, y, &spriteClip);
	}
}
