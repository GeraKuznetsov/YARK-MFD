#pragma once

#include "../Engine/Window.h"
#include "Widget.h"
#include "../Client/Client.h"

#define CONSOLE_WIDTH 80

std::vector<std::string> split(const std::string s, char delim);

struct conLine {
	std::string text;
	int color;
};

class Console : public Widget{
	Font* f;
	std::vector<conLine> lines;
	char type[CONSOLE_WIDTH];
	char curPos;
public:
	Console(WidgetStuff ws);
	void command(std::string com);
	void commandDetach(std::string com);
	void Tick(Draw* draw);
	void DispLine(std::string text, char col);
};