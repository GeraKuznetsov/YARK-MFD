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

Client client;
Window* win;
Font* f;

std::vector<Widget*> widgets;

#define DEFUALT_WIDTH 800
#define DEFUALT_HEIGHT 600

Console* console;

#include "Reg.h"
std::map<std::string, int> Registry;
int RegInt(std::string key, int defualt) {
	if (Registry.find(key) == Registry.end()) {
		Registry[key] = defualt;
		return defualt;
	}
	else {
		return Registry.at(key);
	}
	return Registry[key];
	/*int val = Registry[key];
	if (!val.compare("")) {
		Registry[key] = std::to_string(defualt);
		return defualt;
	}
	return std::stoi(val);*/
}
#include "AltiMeter.h"

Draw* d;
#include "JoyStick.h"

bool OnExit() {
	if (RegInt("ENABLE_SAVE_ON_EXIT", 1)) {
		console->command("savestate");
	}
	return false;
}

void Tick(float delta, Draw* draw) {
	VesselPacket VP = client.vesselPacket;

	RadioAltimeterTick();

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
		widgets[i]->focus = (i == widgets.size() - 1) ? 1 : 0;
		widgets[i]->Tick(draw);
	}
	glScissor(0, 0, win->size.x, win->size.y);
	d = draw;
	JoyStickTick(delta);


	if (client.state == TCPCLIENT_FAILED) {
		console->DispLine("client disconnected: " + client.error, 1);
	}
	else if (client.state == TCPCLIENT_CONNECTED) {
		client.SendControls();
	}
	if (win->MouseClicked(SDL_BUTTON_LEFT)) {
		//printf("click\n");
	}
	SDL_SetWindowTitle(win->gWindow, std::to_string(win->FPS).c_str());
}

//void runExe();

#include "Widgets/NavBall.h"

void main() {
	//runExe();
	//return;
	int error = 0;
	XY size = XY{ DEFUALT_WIDTH,DEFUALT_HEIGHT };

	win = new Window(size, 0, &error);
	if (error) {
		std::cout << "error opening SDL window\n";
		return;
	}
	win->onExit = &OnExit;
	win->SetTargetFPS(600);
	f = new Font(16, 16, "C:\\Windows\\Fonts\\arial.ttf");
	client = Client();

	console = new Console(WidgetStuff{ XY{ 0,0 }, XY{ size.x,size.y }, "Console", f, win, &client, new TextureLoader(),"console" });
	widgets.push_back(console);

	if (!win->HasJoyStick()) {
		Registry["ENABLE_FLYBYWIRE"] = 0;
	}

	console->command("config config.txt");
	OpenPlayer();
	for (int i = 0; i < WARNING_ALTADTUDES; i++) {
		warn_altitudes_sounds[i] = new Sound("Sound/" + std::to_string(warn_altitudes[i]) + ".wav");
	}
	win->Run(&Tick);
}

