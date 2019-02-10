#include "Console.h"
#include "NavBall.h"
#include "AirMap.h"
#include "AtitudeIndicator.h"
#include "SoyuzNavball.h"
#include "Settings.h"
#include "VInfo.h"
#include "LaunchAss.h"
#include "Control.h"
#include "AirPlaneAutoPilot.h"
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
					out = "set " + x.first + " " + std::to_string(x.second) + "\n";
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
				out = "clr\n";
				output.write(out.c_str(), out.size());
				output.close();
				DispLine("Done", 2);
			}
			else {
				DispLine("Invalid Syntex, use: \"savestate [output file]\"", 1);
			}
		}
		else if (!elems[0].compare("connect")) {
			if (elems.size() == 3) {
				if (!client->Connected()) {
					client->Connect(IP = elems[1], PORT = elems[2]);
				}
				//					(*client) = new Client(IP = elems[1], PORT = elems[2]);
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
		else if (!elems[0].compare("get")) {
			if (elems.size() == 2) {
				DispLine(std::to_string(Registry[elems[1]]), 2);
			}
			else {
				DispLine("Invalid Syntex, use: \"get <key>\"", 1);
			}
		}
		else if (!elems[0].compare("set")) {
			if (elems.size() == 3) {
				Registry[elems[1]] = std::stoi(elems[2]);
			}
			else {
				DispLine("Invalid Syntex, use: \"set <key> <val>\"", 1);
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
					widgets.push_back(new NavBall(WidgetStuff{ pos, size, "NavBall", f, win , client, TL,"navball" }));
				}
				else if (!elems[1].compare("airmap")) {
					widgets.push_back(new AirMap(WidgetStuff{ pos, size, "AirMap", f, win, client, TL,"airmap" }));
				}
				else if (!elems[1].compare("ati-in")) {
					widgets.push_back(new AtitudeIndicator(WidgetStuff{ pos, size, "AtitudeIndicator", f, win, client, TL,"ati-in" }));
				}
				else if (!elems[1].compare("soyuznavball")) {
					widgets.push_back(new SoyuzNavBall(WidgetStuff{ pos, size, "SoyuzNavBall", f, win, client, TL,"soyuznavball" }));
				}
				else if (!elems[1].compare("settings")) {
					widgets.push_back(new Settings(WidgetStuff{ pos, size, "Settings", f, win, client, TL,"settings" }));
				}
				else if (!elems[1].compare("vinfo")) {
					widgets.push_back(new VInfo(WidgetStuff{ pos, size, "Vessel Info", f, win, client, TL,"vinfo" }));
				}
				else if (!elems[1].compare("launchass")) {
					widgets.push_back(new LaunchAss(WidgetStuff{ pos, size, "Launch Assistent", f, win, client, TL,"launchass" }));
				}
				else if (!elems[1].compare("controller")) {
					widgets.push_back(new Control(WidgetStuff{ pos, size, "controller", f, win, client, TL,"controller" }));
				}
				else if (!elems[1].compare("autopilot")) {
					widgets.push_back(new AirPlaneAutoPilot(WidgetStuff{ pos, size, "Airplane AutoPilot", f, win, client, TL,"autopilot" }));
				}
				else {
					DispLine("Unkown widget, type \"widgetlist\" for a list of widgets", 1);
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
		else if (!elems[0].compare("widgetlist")) {
			DispLine("Widget List:", 2);
			DispLine("\"navball\" - NavBall", 0);
			DispLine("\"launchass\" - Launch Assistent", 0);
			DispLine("\"airmap\" - Airmap of current planet", 0);
			DispLine("\"ati-in\" - Attitude Indicator for planes", 0);
			DispLine("\"soyuznavball\" - Globe of current planet", 0);
			DispLine("\"settings\" - Settings", 0);
			DispLine("\"vinfo\" - Basic vessel info", 0);
			DispLine("\"controller\" - Vessel controller", 0);
			DispLine("\"autopilot\" - Airplane AutoPilot", 0);
			DispLine("Done", 2);
		}
		else if (!elems[0].compare("clr")) {
			lines.clear();
			memset(type, 0, CONSOLE_WIDTH);
			curPos = 0;
		}
		else if (!elems[0].compare("help")) {
			DispLine("Command List:", 2);
			DispLine("\"open <widget name> [<width> <height> / <pos x> <pos y> <width> <height>]\" - Opens new wiget window", 0);
			DispLine("\"widgetlist\" - Displays a list of widgets", 0);
			DispLine("\"close <name>\" - Closes Widget Window", 0);
			DispLine("\"resize <name> <pos x> <pos y> <width> <height>\" - Resizes Window", 0);
			DispLine("\"set <key> <val>\" - Sets registry key to value", 0);
			DispLine("\"get <key>\" - Displays value of registry key", 0);
			DispLine("\"connect <ip> <port>\" - Connects to a YARK plugin instance", 0);
			DispLine("\"savestate [output file]\" - Saves current state as a command list", 0);
			DispLine("\"config <config file>\" - Excecutes commands in file", 0);
			DispLine("\"winsize <width> <height>\" - Resizes main window", 0);
			DispLine("\"clr\" - Clear Console", 0);
			DispLine("\"help\" - Displays this list", 0);
			DispLine("Done", 2);
		}
		else {
			DispLine("Invalid Command, type \"help\" for help", 1);
		}
	}

}
void Console::commandDetach(std::string com) {
	std::thread t(&Console::command, this, com);
	t.detach();
}
Console::Console(WidgetStuff ws) :Widget(ws) {
	f = new Font(16, 16, "C:\\Windows\\Fonts\\lucon.ttf");
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
	int heighOfLine = 14;
	int y = pos.y + size.y - heighOfLine - 2;
	for (int i = lines.size() - 1; i >= 0; i--) {
		conLine line = lines[i];
		int col = line.color;
		if (col == 0) {
			draw->SetTextColor(1, 1, 1);
		}
		else if (col == 1) {
			draw->SetTextColor(1, 0, 0);
		}
		else if (col == 2) {
			draw->SetTextColor(0, 1, 0);
		}
		draw->DrawString(f, line.text, pos.x + 2, y);
		y -= heighOfLine;
		if (y < pos.y)break;
	}

	draw->SetTextColor(1, 1, 1);
	std::string input = std::string(type);
	draw->DrawString(f, input, pos.x + 2, pos.y + size.y - 3);
	draw->DrawChar(f, '_', pos.x + f->GetTextWidth(input) + 2, pos.y + size.y - 3);
}
void Console::DispLine(std::string text, char col) {
	lines.push_back(conLine{ text,col });
}