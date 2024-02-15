#include "menu.h"

TextBox::TextBox() {
	text = "";
	textColor = { 0,0,0 };
	box = new SDL_Rect;

	box->x = 0;
	box->y = 0;
	box->w = 100;
	box->h = 100;
}

TextBox::TextBox(std::string tex) {
	text = tex;
	textColor = { 0,0,0 };
	SDL_assert(DefaultFont != NULL);
	TextBox::setTexture(DefaultFont);

	box = new SDL_Rect;
	box->x = 0;
	box->y = 0;
	box->w = 100;
	box->h = 100;
}

TextBox::TextBox(std::string tex, TTF_Font* font, SDL_Color color = { 0,0,0 }) {
	text = tex;
	textColor = color;
	TextBox::setTexture(font);

	box = new SDL_Rect;
	box->x = 0;
	box->y = 0;
	box->w = 100;
	box->h = 100;
}

TextBox::TextBox(std::string tex, TTF_Font* fon, int xPos, int yPos, int width, int height, SDL_Color color = { 0,0,0 }) {
	TextBox::text = tex;
	textColor = color;
	font = fon;
	TextBox::setTexture(font);

	box = new SDL_Rect;
	box->x = xPos;
	box->y = yPos;
	box->w = width;
	box->h = height;
}

TextBox::~TextBox() {
	if (box != NULL)
		delete box;
}

SDL_Color TextBox::getTextColor() {
	return textColor;
}

SDL_Rect* TextBox::getRect() {
	return box;
}

SDL_Texture* TextBox::getTexture() {
	return texture;
}

void TextBox::resetTexture() {
	setTexture(font);
}

void TextBox::setPosition(int x, int y) {
	box->x = x;
	box->y = y;
}

void TextBox::setDimensions(int w, int h) {
	box->w = w;
	box->h = h;
}

void TextBox::setTextColor(SDL_Color color) {
	textColor = color;
	resetTexture();
}

void TextBox::setText(std::string tex, TTF_Font* newFont) {
	text = tex;
	font = newFont;

	TextBox::setTexture(newFont);
}

void TextBox::setTexture(TTF_Font* font) {
	// create the text texture
	SDL_Surface* surfaceMessage = TTF_RenderText_Blended(font, text.c_str(), textColor);
	texture = SDL_CreateTextureFromSurface(renderer, surfaceMessage);
}

void TextBox::render(SDL_Renderer* renderer) {
	SDL_RenderCopy(renderer, texture, NULL, box);
}

Button::Button() {
	behavior = nullptr;
	label = nullptr;

	position.x = 0;
	position.y = 0;
	width = 0;
	height = 0;

	box.x = 0;
	box.y = 0;
	box.w = 0;
	box.h = 0;
}

Button::Button(std::string buttonText, SDL_Point pos, int w, int h) {
	behavior = nullptr;

	position = pos;
	width = w;
	height = h;

	label = new TextBox(buttonText, DefaultFont, position.x, position.y, width, height);

	box.x = position.x - 50;
	box.y = position.y - 10;
	box.w = width + 100;
	box.h = height + 20;
}

TextBox* Button::getLabel() {
	return label;
}

SDL_Rect Button::getRect() {
	return box;
}

SDL_Point Button::getPosition() {
	return position;
}

int Button::getID() {
	return id;
}

void Button::setID(int newID) {
	id = newID;
}

// set the behavior of the button
void Button::setBehavior(void(*func)()) {
	behavior = func;
}

void Button::setHighlighted(bool value) {
	highlighted = value;
}

void Button::handleEvent() {
	behavior();
}

bool Button::IsHighlighted() {
	return highlighted;
}

Menu::Menu() {
	outline = nullptr;
	position.x = 0;
	position.y = 0;
	width = 0;
	height = 0;

	textbox = new TextBox();
}

Menu::Menu(int x, int y, int w, int h, std::string text) {
	position.x = x;
	position.y = y;
	width = w;
	height = h;

	textbox = new TextBox();
	textbox->setText(text, DefaultFont);
	textbox->setPosition(x, y);
	textbox->setDimensions(w, h);

	outline = new SDL_Rect();
	outline->x = x;
	outline->y = y;
	outline->w = w;
	outline->h = h;
}

void Menu::addButton(Button button) {
	int newID = (int)buttons.size();
	button.setID(newID);
	buttons.push_back(button);
}

SDL_Rect* Menu::getRect() {
	return outline;
}

SDL_Point Menu::getPos() {
	return position;
}

void Menu::setText(std::string text) {
	textbox->setText(text, DefaultFont);
}

void Menu::setText(std::string text, TTF_Font* font) {
	textbox->setText(text, font);
}

void Menu::resetTexture() {
	textbox->resetTexture();
}

// sets the position of the menu's textbox; uses floats to set position based on a scaling of the menu's dimensions
void Menu::setTextPosition(float x, float y) {
	if (x > 1 || x <= 0 || y > 1 || y <= 0) {
		printf("text position can only be set to to the range (0,1]!\n");
		return;
	}

	int menuX = position.x;
	int menuY = position.y;
	int menuW = width;
	int menuH = height;

	int newPosXCoord = (int)(x * menuW) + menuX;
	int newPosYCoord = (int)(y * menuH) + menuY;

	textbox->setPosition(newPosXCoord, newPosYCoord);
	this->resetTexture();
}

// sets the dimensions of the menu's textbox; uses floats to set dimensions based on a scaling of the menu's dimensions
void Menu::setTextDimensions(float w, float h) {
	if (w > 1 || w <= 0 || h > 1 || h <= 0) {
		printf("text position can only be set to to the range (0,1]!\n");
		return;
	}

	int menuW = width;
	int menuH = height;

	int newWidth = (int)(w * menuW);
	int newHeight = (int)(h * menuH);

	textbox->setDimensions(newWidth, newHeight);
	this->resetTexture();
}

void Menu::render(SDL_Renderer* renderer) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(renderer, outline);
	SDL_RenderCopy(renderer, textbox->getTexture(), NULL, textbox->getRect());

	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	for (int i = 0; i < buttons.size(); i++) {
		SDL_Rect rect = buttons[i].getRect();
		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x, rect.y + rect.h);

		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);
		SDL_RenderDrawLine(renderer, rect.x, rect.y, rect.x + rect.w, rect.y);

		SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x + rect.w, rect.y, rect.x + rect.w, rect.y + rect.h);

		SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);
		SDL_RenderDrawLine(renderer, rect.x, rect.y + rect.h, rect.x + rect.w, rect.y + rect.h);

		Button button = buttons[i];
		if (button.IsHighlighted()) {
			SDL_Rect buttonRect = button.getRect();
			SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
			SDL_RenderFillRect(renderer, &buttonRect);

			TextBox* tb = button.getLabel();
			SDL_Color currentColor = tb->getTextColor(); // save current color to set it back later
			SDL_Color highlightColor = { 255, 255, 255 };
			tb->setTextColor(highlightColor);
			SDL_RenderCopy(renderer, tb->getTexture(), NULL, tb->getRect());
			tb->setTextColor(currentColor);
		}
		else {
			TextBox* box = button.getLabel();
			SDL_RenderCopy(renderer, box->getTexture(), NULL, box->getRect());
		}
	}
}

void handleMenuButtons(Button* button) {
	button->handleEvent();
}

void Menu::handleEvent(SDL_Event* e) {
	// get mouse position
	int x, y;
	SDL_GetMouseState(&x, &y);

	if (e->type == SDL_MOUSEMOTION) {
		for (int i = 0; i < this->buttons.size(); i++) {
			// get the individual button
			Button* button = &this->buttons[i];
			//SDL_Point buttonPos = button.getPosition();
			SDL_Rect buttonProps = button->getRect();

			// check if mouse inside of button
			bool mouseIsInsideButton = true;

			// mouse is left of cell
			if (x < buttonProps.x)
				mouseIsInsideButton = false;
			// mouse is right of cell
			else if (x > buttonProps.x + buttonProps.w)
				mouseIsInsideButton = false;
			// mouse is above button
			else if (y < buttonProps.y)
				mouseIsInsideButton = false;
			// mouse is below the button
			else if (y > buttonProps.y + buttonProps.h)
				mouseIsInsideButton = false;

			if (mouseIsInsideButton) {
				button->setHighlighted(true);
			}
			else {
				button->setHighlighted(false);
			}
		}
	}
	else if (e->type == SDL_MOUSEBUTTONDOWN) {
		for (int i = 0; i < this->buttons.size(); i++) {
			// get the individual button
			Button* button = &this->buttons[i];
			//SDL_Point buttonPos = button.getPosition();
			SDL_Rect buttonProps = button->getRect();

			// check if mouse inside of button
			bool mouseIsInsideButton = true;

			// mouse is left of cell
			if (x < buttonProps.x)
				mouseIsInsideButton = false;
			// mouse is right of cell
			else if (x > buttonProps.x + buttonProps.w)
				mouseIsInsideButton = false;
			// mouse is above button
			else if (y < buttonProps.y)
				mouseIsInsideButton = false;
			// mouse is below the button
			else if (y > buttonProps.y + buttonProps.h)
				mouseIsInsideButton = false;

			if (mouseIsInsideButton) {
				button->setHighlighted(false);
				handleMenuButtons(button);
			}
		}
	}

}
