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

#include "Client/Client.h"
#include "Engine/Window.h"
#include "Widgets/Widget.h"
#include "Widgets/Console.h"
#include "Engine/Sound.h"

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

bool clientDisc;

bool smart_ass_was_on;

void Tick(float delta, Draw* draw) {
	VesselPacket VP = client.Vessel;

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


	if (client.GetState() == TCP_FAILED) {
		if (!clientDisc) {
			console->DispLine("client disconnected: " + std::string(client.error), 1);
			clientDisc = true;
		}
	}
	else {
		clientDisc = false;
		if (client.GetState() == TCP_CONNECTED) {
			if (client.Vessel.SASMode == SAS_HOLD_VECTOR) {
				if (!smart_ass_was_on) {
					smart_ass_was_on = true;
					Registry["SASS_PITCH"] = client.Vessel.Pitch;
					Registry["SASS_HEADING"] = client.Vessel.Heading;
					Registry["SASS_ROLL"] = client.Vessel.Roll;
				}
				client.Control.SetSASHoldVector(Registry["SASS_PITCH"], Registry["SASS_HEADING"], Registry["SASS_ROLL"]);
			}
			else {
				smart_ass_was_on = false;
				client.Control.ReSetSASHoldVector();
			}
			client.Control.InputThrottle(Registry["THROTTLE"]);
			client.SendControls();
		}
	}
	SDL_SetWindowTitle(win->gWindow, std::to_string(win->FPS).c_str());
}

int main() {
	int error = 0;
	XY size = XY{ DEFUALT_WIDTH,DEFUALT_HEIGHT };

	win = new Window(size, 0, &error);
	if (error) {
		std::cout << "error opening SDL window\n";
		return 1;
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
	return 0;
}
