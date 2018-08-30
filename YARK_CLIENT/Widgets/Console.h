#pragma once

#include "../Engine/Window.h"
#include "Widget.h"
#include "../Client/Client.h"

#define CONSOLE_WIDTH 40
#define CONSOLE_HEIGHT 20
#define CONSOLE_FONT_SIZE 14

std::vector<std::string> split(const std::string s, char delim);

class Console : public Widget{
	Font* f;
	char data[CONSOLE_WIDTH * CONSOLE_HEIGHT];
	char type[CONSOLE_WIDTH];
	char color[CONSOLE_WIDTH * CONSOLE_HEIGHT];
	char curPos;
	bool focus;
public:
	Console(WidgetStuff ws);
	void command(std::string com);
	void commandDetach(std::string com);
	void Tick(Draw* draw);
	void DispLine(std::string text, char col);
};