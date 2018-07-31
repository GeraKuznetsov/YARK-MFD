#include "Console.h"
#include "NavBall.h"

#include <sstream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <thread>
#include <iostream>

std::vector<std::string> split(const std::string s, char delim) {
	std::istringstream iss(s);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{} };
	return tokens;
}

Widget* getWidget(std::string s, std::vector<Widget*> *widgets) {
	for (int i = 0; i < widgets->size(); i++) {
		std::string tit = (*widgets)[i]->title;
		std::transform(tit.begin(), tit.end(), tit.begin(), ::tolower);
		if (!tit.compare(s)) {
			return (*widgets)[i];
		}
	}
	return 0;
}

void Console::command(std::string com) {
	DispLine(">" + com);
	colorCur = 0;
	std::vector<std::string> elems = split(com, ' ');
	if (!elems[0].compare("connect")) {
		DispLine("connecting...");
		(*client) = new Client(elems[1], elems[2]);
		while ((*client)->state == TCPCLIENT_CONNECTING) {

		}
		if ((*client)->state == TCPCLIENT_CONNECTED) {
			colorCur = 2;
			DispLine("conneced");
		}
		else if ((*client)->state == TCPCLIENT_FAILED) {
			colorCur = 1;
			DispLine("error: " + (*client)->error);
			delete (*client);
			(*client) = NULL;
		}
		colorCur = 0;
	}
	else if (!elems[0].compare("config")) {
		std::ifstream in(elems[1]);
		if (!in) {
			std::cout << "Cannot open input file.\n";
		}
		std::string str;
		while (std::getline(in, str)) {
			command(str);
		}
	}
	else if (!elems[0].compare("winsize")) {
		XYi size;
		size.x = std::stoi(elems[1]);
		size.y = std::stoi(elems[2]);
		w->SetSize(size);
	}
	else if (!elems[0].compare("widget")) {
		if (!elems[1].compare("open")) {
			XY pos;
			pos.x = std::stoi(elems[3]);
			pos.y = std::stoi(elems[4]);
			XY size;
			size.x = std::stoi(elems[5]);
			size.y = std::stoi(elems[6]);
			if (!elems[2].compare("navball")) {
				NavBall* navball = new NavBall(pos, size, "NavBall", f, client);
				navball->win = w;
				widgets->push_back(navball);
			}
		}
		else if (!elems[1].compare("resize")) {
			XY pos;
			pos.x = std::stoi(elems[3]);
			pos.y = std::stoi(elems[4]);
			XY size;
			size.x = std::stoi(elems[5]);
			size.y = std::stoi(elems[6]);
			Widget* w = getWidget(elems[2], widgets);
			if (w) {
				w->Resize(pos, size);
			}
			else {
				std::cout << "null\n";
			}
		}
	}
}
void Console::commandDetach(std::string com) {
	std::thread t(&Console::command, this, com);
	t.detach();
}
Console::Console(XY pos, XY size, std::string title, Font* font, Window* win, Client** client, std::vector<Widget*> *widgets) :Widget(pos, size, title, font) {
	w = win;
	this->widgets = widgets;
	this->client = client;
	f = new Font(16, 16, "C:\\Windows\\Fonts\\lucon.ttf");
	for (int y = 0; y < CONSOLE_HEIGHT; y++) {
		for (int x = 0; x < CONSOLE_WIDTH; x++) {
			data[y*CONSOLE_WIDTH + x] = ' ';
			color[y*CONSOLE_WIDTH + x] = 0;
		}
	}
	memset(type, 0, CONSOLE_WIDTH);
	colorCur = 0;
	curPos = 0;
}
void Console::Tick(Draw* draw) {
	RenderWindow(draw);
	/*	if (curPos < CONSOLE_WIDTH) {
		for (int i = SDL_SCANCODE_A; i <= SDL_SCANCODE_Z; i++) {
			if (w->KeyRepeating(i)) {
				type[curPos] = (i - SDL_SCANCODE_A + 'A');
				curPos++;
			}
		}
		for (int i = SDL_SCANCODE_0; i <= SDL_SCANCODE_9; i++) {
			if (w->KeyRepeating(i)) {
				type[curPos] = (i - SDL_SCANCODE_0 + '0');
				curPos++;
			}
		}
		if (w->KeyRepeating(SDL_SCANCODE_SPACE)) {
			type[curPos] = ' ';
			curPos++;
		}
	}
	if (curPos != 0) {
		if (w->KeyRepeating(SDL_SCANCODE_BACKSPACE)) {
			curPos--;
			type[curPos] = 0;
		}
		if (w->KeyTyped(SDL_SCANCODE_RETURN)) {
			command(std::string(type));
			memset(type, 0, CONSOLE_WIDTH);
			curPos = 0;
		}
	}*/

	draw->BindTextShader();
	for (int y = 0; y < CONSOLE_HEIGHT; y++) {
		for (int x = 0; x < CONSOLE_WIDTH; x++) {
			char col = color[(CONSOLE_HEIGHT - 1 - y)*CONSOLE_WIDTH + x];
			if (col == 0) {
				draw->SetTextColor(1, 1, 1);
			}
			else if (col == 1) {
				draw->SetTextColor(1, 0, 0);
			}
			else if (col == 2) {
				draw->SetTextColor(0, 1, 0);
			}
			draw->DrawChar(f, data[(CONSOLE_HEIGHT - 1 - y)*CONSOLE_WIDTH + x], pos.x + x * CONSOLE_FONT_SIZE, pos.y + size.y - y * CONSOLE_FONT_SIZE);
		}
	}

	draw->SetTextColor(1, 1, 1);
	int x;
	for (x = 0; x < CONSOLE_WIDTH; x++) {
		if (type[x] == 0) {
			break;
		}
		draw->DrawChar(f, type[x], pos.x + x * CONSOLE_FONT_SIZE, pos.y + CONSOLE_HEIGHT * CONSOLE_FONT_SIZE);
	}
	draw->DrawChar(f, '_', pos.x + x * CONSOLE_FONT_SIZE, pos.y + CONSOLE_HEIGHT * CONSOLE_FONT_SIZE);
}
void Console::DispLine(std::string text) {
	memcpy(data, data + CONSOLE_WIDTH, CONSOLE_HEIGHT*CONSOLE_WIDTH - CONSOLE_WIDTH);
	memset(data + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, ' ', CONSOLE_WIDTH);
	memcpy(data + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, text.data(), text.length());

	memcpy(color, color + CONSOLE_WIDTH, CONSOLE_HEIGHT*CONSOLE_WIDTH - CONSOLE_WIDTH);
	memset(color + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, 0, CONSOLE_WIDTH);
	memset(color + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, colorCur, text.length());
}