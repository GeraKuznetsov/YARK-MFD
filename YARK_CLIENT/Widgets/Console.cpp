#include "Console.h"
#include "NavBall.h"
#include "AirMap.h"
#include "AtitudeIndicator.h"
#include "SoyuzNavball.h"
#include "Settings.h"
#include "VInfo.h"
#include <sstream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <thread>
#include <iostream>
#include <string>

extern std::vector<Widget*> widgets;

std::vector<std::string> split(const std::string s, char delim) {
	std::istringstream iss(s);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{} };
	return tokens;
}

Widget* getWidget(std::string s) {
	for (int i = 0; i < widgets.size(); i++) {
		std::string tit = widgets[i]->title;
		std::transform(tit.begin(), tit.end(), tit.begin(), ::tolower);
		if (!tit.compare(s)) {
			return widgets[i];
		}
	}
	return 0;
}

#include "../Reg.h"

std::string IP, PORT;

void Console::command(std::string com) {
	DispLine(">" + com, 0);
	if (com[0] != '#') {
		std::vector<std::string> elems = split(com, ' ');
		if (!elems[0].compare("config")) {
			if (elems.size() == 2) {
				std::ifstream in(elems[1]);
				if (!in) {
					std::cout << "Cannot open input file.\n";
				}
				std::string str;
				while (std::getline(in, str)) {
					command(str);
				}
			}
			else {
				DispLine("Invalid Syntex, use: \"config <config file>\"", 1);
			}
		}
		else if (!elems[0].compare("savestate")) {
			std::string path = "config.txt";
			if (elems.size() == 2) {
				path = elems[1];
			}
			if (elems.size() == 1 || elems.size() == 2) {
				std::ofstream output(path);
				std::string out;
				out = "connect " + (IP)+" " + (PORT)+"\n";
				output.write(out.c_str(), out.size());
				XY size = win->getSize();
				out = "winsize " + std::to_string(size.x) + " " + std::to_string(size.y) + "\n";
				output.write(out.c_str(), out.size());
				for (std::map<std::string, int>::value_type& x : Registry)
				{
					out = "reg " + x.first + " " + std::to_string(x.second) + "\n";
					output.write(out.c_str(), out.size());
				}
				for (int i = 0; i < widgets.size(); i++) {
					if ((void*)(widgets[i]) == (void*)this) {
						out = "resize " + getSaveParams();
					}
					else {
						out = "open " + widgets[i]->getSaveParams();
					}
					output.write(out.c_str(), out.size());
				}
				output.close();
				DispLine("Done", 2);
			}
			else {
				DispLine("Invalid Syntex, use: \"savestate [output file]\"", 1);
			}
		}
		else if (!elems[0].compare("connect")) {
			if (elems.size() == 3) {
				if (!(*client)) {
					(*client) = new Client(IP = elems[1], PORT = elems[2]);
				}
			}
			else {
				DispLine("Invalid Syntex, use: \"connect <ip> <port>\"", 1);
			}
		}
		else if (!elems[0].compare("winsize")) {
			if (elems.size() == 3) {
				win->SetSize(XY{ std::stoi(elems[1]), std::stoi(elems[2]) });
			}
			else {
				DispLine("Invalid Syntex, use: \"winsize <width> <height>\"", 1);
			}
		}
		else if (!elems[0].compare("reg")) {
			if (elems.size() == 3) {
				Registry[elems[1]] = std::stoi(elems[2]);
			}
			else if (elems.size() == 2) {
				DispLine(std::to_string(Registry[elems[1]]), 2);
			}
			else {
				DispLine("Invalid Syntex, use: \"reg <key> [new val]\"", 1);
			}
		}
		else if (!elems[0].compare("open")) {
			XY pos = XY{ win->getSize().x / 4, win->getSize().y / 4 };
			XY size = XY{ win->getSize().x / 2,win->getSize().y / 2 };
			if (elems.size() == 6) {
				pos.x = std::stoi(elems[2]);
				pos.y = std::stoi(elems[3]);

				size.x = std::stoi(elems[4]);
				size.y = std::stoi(elems[5]);
			}
			else if (elems.size() == 4) {
				size.x = std::stoi(elems[2]);
				size.y = std::stoi(elems[3]);
			}
			if (elems.size() == 6 || elems.size() == 4 || elems.size() == 2) {
				if (!elems[1].compare("navball")) {
					NavBall* navball = new NavBall(WidgetStuff{ pos, size, "NavBall", f, win , client, TL,"navball" });
					widgets.push_back(navball);
				}
				else if (!elems[1].compare("airmap")) {
					AirMap* ag = new AirMap(WidgetStuff{ pos, size, "AirMap", f, win, client, TL,"airmap" });
					widgets.push_back(ag);
				}
				else if (!elems[1].compare("ati-in")) {
					AtitudeIndicator* navball = new AtitudeIndicator(WidgetStuff{ pos, size, "AtitudeIndicator", f, win, client, TL,"ati-in" });
					widgets.push_back(navball);
				}
				else if (!elems[1].compare("soyuznavball")) {
					SoyuzNavBall* navball = new SoyuzNavBall(WidgetStuff{ pos, size, "SoyuzNavBall", f, win, client, TL,"soyuznavball" });
					widgets.push_back(navball);
				}
				else if (!elems[1].compare("settings")) {
					Settings* navball = new Settings(WidgetStuff{ pos, size, "Settings", f, win, client, TL,"settings" });
					widgets.push_back(navball);
				}
				else if (!elems[1].compare("vinfo")) {
					VInfo* vi = new VInfo(WidgetStuff{ pos, size, "Vessel Info", f, win, client, TL,"vinfo" });
					widgets.push_back(vi);
				}
			}
			else {
				DispLine("Invalid Syntex, use: \"open <name> [<width> <height> / <pos x> <pos y> <width> <height>]\"", 1);
			}
		}
		else if (!elems[0].compare("close")) {
			if (elems.size() == 2) {
				Widget* w = getWidget(elems[1]);
				if (w) {
					w->close = true;
				}
			}
			else {
				DispLine("Invalid Syntex, use: \"close <name>\"", 1);
			}
		}
		else if (!elems[0].compare("resize")) {
			if (elems.size() == 6) {
				XY pos;
				pos.x = std::stoi(elems[2]);
				pos.y = std::stoi(elems[3]);
				XY size;
				size.x = std::stoi(elems[4]);
				size.y = std::stoi(elems[5]);
				Widget* w = getWidget(elems[1]);
				if (w) {
					w->Resize(pos, size);
				}
				else {
					std::cout << "null\n";
				}
			}
			else {
				DispLine("Invalid Syntex, use: \"resize <name> <pos x> <pos y> <width> <height>\"", 1);
			}
		}
	}

}
void Console::commandDetach(std::string com) {
	std::thread t(&Console::command, this, com);
	t.detach();
}
Console::Console(WidgetStuff ws) :Widget(ws) {
	f = new Font(16, 16, "C:\\Windows\\Fonts\\lucon.ttf");
	for (int y = 0; y < CONSOLE_HEIGHT; y++) {
		for (int x = 0; x < CONSOLE_WIDTH; x++) {
			data[y*CONSOLE_WIDTH + x] = ' ';
			color[y*CONSOLE_WIDTH + x] = 0;
		}
	}
	memset(type, 0, CONSOLE_WIDTH);
	curPos = 0;
}
void Console::Tick(Draw* draw) {
	WindowUpdate(draw);
	if (focus) {
		if (curPos < CONSOLE_WIDTH) {
			for (int i = SDL_SCANCODE_A; i <= SDL_SCANCODE_Z; i++) {
				if (win->KeyRepeating(i)) {
					int add = win->KeyDown(SDL_SCANCODE_LSHIFT) ? 'A' : 'a';
					type[curPos] = (i - SDL_SCANCODE_A + add);
					curPos++;
				}
			}
			for (int i = SDL_SCANCODE_1; i <= SDL_SCANCODE_9; i++) {
				if (win->KeyRepeating(i)) {
					type[curPos] = (i - SDL_SCANCODE_1 + '1');
					curPos++;
				}
			}
			if (win->KeyRepeating(SDL_SCANCODE_0)) {
				type[curPos] = '0';
				curPos++;
			}
			if (win->KeyRepeating(SDL_SCANCODE_PERIOD)) {
				type[curPos] = '.';
				curPos++;
			}
			if (win->KeyRepeating(SDL_SCANCODE_MINUS)) {
				type[curPos] = win->KeyDown(SDL_SCANCODE_LSHIFT) ? '_' : '-';
				curPos++;
			}
			if (win->KeyRepeating(SDL_SCANCODE_SPACE)) {
				type[curPos] = ' ';
				curPos++;
			}
		}
		if (curPos != 0) {
			if (win->KeyRepeating(SDL_SCANCODE_BACKSPACE)) {
				curPos--;
				type[curPos] = 0;
			}
			if (win->KeyTyped(SDL_SCANCODE_RETURN)) {
				command(std::string(type));
				memset(type, 0, CONSOLE_WIDTH);
				curPos = 0;
			}
		}
	}

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
			draw->DrawChar(f, data[(CONSOLE_HEIGHT - 1 - y)*CONSOLE_WIDTH + x], pos.x + x * CONSOLE_FONT_SIZE, pos.y + size.y - y * CONSOLE_FONT_SIZE - CONSOLE_FONT_SIZE);
		}
	}

	draw->SetTextColor(1, 1, 1);
	int x;
	for (x = 0; x < CONSOLE_WIDTH; x++) {
		if (type[x] == 0) {
			break;
		}
		draw->DrawChar(f, type[x], pos.x + x * CONSOLE_FONT_SIZE, pos.y + size.y - 2);
	}
	draw->DrawChar(f, '_', pos.x + x * CONSOLE_FONT_SIZE, pos.y + size.y);
}
void Console::DispLine(std::string text, char col) {
	if (text.length() > CONSOLE_WIDTH) {
		text = text.substr(0, CONSOLE_WIDTH);
	}
	memcpy(data, data + CONSOLE_WIDTH, CONSOLE_HEIGHT*CONSOLE_WIDTH - CONSOLE_WIDTH);
	memset(data + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, ' ', CONSOLE_WIDTH);
	memcpy(data + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, text.data(), text.length());

	memcpy(color, color + CONSOLE_WIDTH, CONSOLE_HEIGHT*CONSOLE_WIDTH - CONSOLE_WIDTH);
	memset(color + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, 0, CONSOLE_WIDTH);
	memset(color + CONSOLE_HEIGHT * CONSOLE_WIDTH - CONSOLE_WIDTH, col, text.length());
}