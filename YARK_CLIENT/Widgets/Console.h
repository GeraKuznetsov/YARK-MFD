#pragma once

#include "../Engine/Window.h"
#include "Widget.h"

#define CONSOLE_WIDTH 40
#define CONSOLE_HEIGHT 20
#define CONSOLE_FONT_SIZE 14

class Console : public Widget{
	Font* f;
	Window* w;
	char data[CONSOLE_WIDTH * CONSOLE_HEIGHT];
	char type[CONSOLE_WIDTH];
	char color[CONSOLE_WIDTH * CONSOLE_HEIGHT];
	char curPos;
public:
	char colorCur;
	Console(Window* win, XY pos);
	void Tick(Draw* draw);
	void DispLine(std::string text);
};