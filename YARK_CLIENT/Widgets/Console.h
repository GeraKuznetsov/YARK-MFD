#pragma once

#include "../Engine/Window.h"
#include "Widget.h"
#include "../Client/Client.h"

#define CONSOLE_WIDTH 40
#define CONSOLE_HEIGHT 20
#define CONSOLE_FONT_SIZE 14

class Console : public Widget{
	Font* f;
	Window* w;
	Client** client;
	char data[CONSOLE_WIDTH * CONSOLE_HEIGHT];
	char type[CONSOLE_WIDTH];
	char color[CONSOLE_WIDTH * CONSOLE_HEIGHT];
	char curPos;
	std::vector<Widget*> *widgets;
public:
	char colorCur;
	Console(XY pos, XY size, std::string title,Font* f, Window* win, Client** client, std::vector<Widget*> *widgets);
	void command(std::string com);
	void commandDetach(std::string com);
	void Tick(Draw* draw);
	void DispLine(std::string text);
};