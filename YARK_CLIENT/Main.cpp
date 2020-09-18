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
#include "Engine/Sound.h"
#include "Widgets/Container.h"
#include "Widgets/Util/IM.h"
#include "Widgets/Util/TextureLoader.h"
Container* topLevel;

Client client;
Window* win;
TextureLoader TL;
Font* f;

#define DEFUALT_WIDTH 800
#define DEFUALT_HEIGHT 600

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

#include "Arduino/EnableArduino.h"
#if ENABLE_ARDUINO
void RunArduino();
#endif 

#include "AltiMeter.h"

Draw* draw;
#include "JoyStick.h"

bool OnExit() {
	if (RegInt("ENABLE_SAVE_ON_EXIT", 1)) {
		//console->command("savestate");
	}
	return false;
}

bool clientDisc;

bool smart_ass_was_on;

void Tick(float delta) {
	VesselPacket VP = client.Vessel;
	glViewport(0, 0, win->size.x, win->size.y);
	draw->UpdateSize(win->size.x, win->size.y);
	RadioAltimeterTick();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	draw->SwitchShader(SHADER_2D);
	draw->SetView2D();
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0, 0, 1);
	draw->DrawRect2D(0, 0, win->size.x, WID_BORDER);
	draw->DrawRect2D(0, 0, WID_BORDER, win->size.y);
	draw->DrawRect2D(win->size.x - WID_BORDER, 0, win->size.x, win->size.y);
	draw->DrawRect2D(0, win->size.y - WID_BORDER, win->size.x, win->size.y);

	topLevel->Update(true, WID_BORDER, WID_BORDER, win->size.x - WID_BORDER * 2, win->size.y - WID_BORDER * 2);
	JoyStickTick(delta);
	//client.Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_IGNORE);
	if (client.GetState() == TCP_FAILED) {
		if (!clientDisc) {
			//console->DispLine("client disconnected: " + std::string(client.error), 1);
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
			//printf("%d\n", client.Control.Throttle);
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
	f = new Font(16, 16, "arial.ttf");
	client = Client();
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	draw = new Draw(size.x, size.y);
	IM::Load();
	LoadWindowIcons();
	ContainerLoadWidgets();
	topLevel = new Container(new Container(optionsWidget), new Container(0));
	OpenPlayer();
	for (int i = 0; i < WARNING_ALTADTUDES; i++) {
		warn_altitudes_sounds[i] = new Sound("Sound/" + std::to_string(warn_altitudes[i]) + ".wav");
	}
#if ENABLE_ARDUINO
	RunArduino();
#endif
	//console->command("config config.txt");
	win->Run(&Tick);
	return 0;
}

