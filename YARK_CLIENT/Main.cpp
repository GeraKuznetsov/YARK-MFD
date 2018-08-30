//dont show le console
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <thread>

#include "Client\Client.h"
#include "Engine\Window.h"
#include "Widgets\Widget.h"
#include "Widgets\Console.h"
#include "Engine\Sound.h"

Client* client;
Window* win;
Font* f;

std::vector<Widget*> widgets;

#define DEFUALT_WIDTH 800
#define DEFUALT_HEIGHT 600

Console* console;

#include "Reg.h"
std::map<std::string, std::string> Registry;
int RegInt(std::string key, int defualt) {
	std::string val = Registry[key];
	if (!val.compare("")) {
		Registry[key] = std::to_string(defualt);
		return defualt;
	}
	return std::stoi(val);
}
#include "AltiMeter.h"
#include "JoyStick.h"

void Tick(float delta, Draw* draw) {
	VesselPacket VP = (client) ? (client)->vesselPacket : VesselPacket();

	RadioAltimeterTick(VP);
	JoyStickTick();

	if (client) {
		if (client->state == TCPCLIENT_FAILED) {
			console->DispLine("client disconnected: " + client->error, 1);
			delete client;
			client = NULL;
		}
		else if (client->state == TCPCLIENT_CONNECTED) {
			client->SendControls();
		}
	}

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);

	for (int i = widgets.size() - 1; i >= 0; i--) {
		if (int mode = widgets[i]->Input()) {
			std::vector<Widget*> newOrder;
			for (int x = 0; x < widgets.size(); x++) {
				if (x != i) {
					newOrder.push_back(widgets[x]);
				}
			}
			if (mode != 2) newOrder.push_back(widgets[i]);
			widgets = newOrder;
			break;
		}
	}

	for (int i = 0; i < widgets.size(); i++) {
		glScissor(0, 0, win->size.x, win->size.y);
		widgets[i]->Tick(draw);
	}
	glScissor(0, 0, win->size.x, win->size.y);
	win->MouseClicked(SDL_BUTTON_LEFT);
	SDL_SetWindowTitle(win->gWindow, std::to_string(win->FPS).c_str());
}

void main() {
	int error = 0;
	std::string IP = "localhost", port = "9998";
	XY size = XY{ DEFUALT_WIDTH,DEFUALT_HEIGHT };

	std::ifstream in("config.txt");
	if (!in) {
		std::cout << "Cannot open input file.\n";
	}
	std::string str;
	while (std::getline(in, str)) {
		std::vector<std::string> elems = split(str, ' ');
		if (!elems[0].compare("IP")) {
			IP = elems[1];
			port = elems[2];
		}
		else if (!elems[0].compare("winsize")) {
			size.x = std::stoi(elems[1]);
			size.y = std::stoi(elems[2]);
		}
	}

	win = new Window(size, 0, &error);
	if (error) {
		std::cout << "error opening SDL window\n";
		return;
	}

	//win->SetTargetFPS(600);
	f = new Font(16, 16, "C:\\Windows\\Fonts\\arial.ttf");

	//cam = new Cam(0, 0, glm::vec3{ 0,0,0 });
	//cam->fov = glm::radians(45.f);
	//cam->orthro = true;

	console = new Console(WidgetStuff{ XY{ 0,0 }, XY{ size.x,size.y }, "Console", f, win, &client, new TextureLoader(),"console" });
	widgets.push_back(console);
	client = new Client(IP, port);



	//console->DispLine("test ", 2);
	console->command("config start.txt");
	OpenPlayer();
	for (int i = 0; i < WARNING_ALTADTUDES; i++) {
		warn_altitudes_sounds[i] = new Sound("Sound/" + std::to_string(warn_altitudes[i]) + ".wav");
	}
	win->Run(&Tick);


}

