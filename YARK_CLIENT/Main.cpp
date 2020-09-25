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
#include "Wrap/Window.h"
#include "Wrap/Sound.h"
#include "Widgets/Container.h"
#include "Widgets/Util/IM.h"
#include "Widgets/Util/TextureLoader.h"
#include "Log.h"
Container* topLevel;

Client client;
Window* win;
TextureLoader TL;
Font* f;

std::string IP = "127.0.0.1";
std::string PORT = "9999";

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
}

#include "Arduino/EnableArduino.h"
#if ENABLE_ARDUINO
void RunArduino();
#endif 

#include "AltiMeter.h"

Draw* draw;
#include "JoyStick.h"

std::string config = "config.txt";

bool OnExit() {
	std::ofstream output(config);
	std::string out;

	for (std::map<std::string, int>::value_type& x : Registry)
	{
		out = x.first + "=" + std::to_string(x.second) + "\n";
		output.write(out.c_str(), out.size());
	}
	out = "IP = " + IP + "\n";
	output.write(out.c_str(), out.size());
	out = "PORT = " + PORT;
	output.write(out.c_str(), out.size());
	output.close();
	return false;
}

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

	Log log = Log{
		client.Vessel.MissionTime,
		{
			client.Vessel.G,
			client.Vessel.Alt,
			client.Vessel.Vsurf,
			client.Vessel.VOrbit,
			float(client.Vessel.CurrentStage)},
	};
	UpdateLog(&log);

	client.Control.SetControlerMode(CONTROLLER_ROT, AXIS_IGNORE);
	client.Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_IGNORE);
	client.Control.SetControlerMode(CONTROLLER_TRANS, AXIS_IGNORE);

	if (topLevel->Update(true, WID_BORDER, WID_BORDER, win->size.x - WID_BORDER * 2, win->size.y - WID_BORDER * 2)) win->running = false;
		
	JoyStickTick(delta);
	{
		int  num = -1;
		for (int i = SDL_SCANCODE_1; i <= SDL_SCANCODE_9; i++) {
			if (win->KeyTyped(i)) {
				num = i - SDL_SCANCODE_1 + 1;
			}
		}
		if (win->KeyTyped(SDL_SCANCODE_0)) {
			num = 0;
		}
		if (num != -1) {
			std::string filename = "layout" + std::to_string(num) + ".txt";
			if (win->KeyDown(SDL_SCANCODE_F1)) {
				printf("Saving to %s\n", filename.c_str());
				std::ofstream output(filename);
				std::string out = topLevel->Save();
				output.write(out.c_str(), out.size());
				output.close();
			}
			else if (win->KeyDown(SDL_SCANCODE_F2)) {
				std::ifstream in(filename);
				if (in) {
					printf("Loading From %s\n", filename.c_str());
					topLevel->Free();
					delete topLevel;
					std::string data;
					std::getline(in, data);
					topLevel = new Container(data.substr(1, data.length() - 2));
				}
			}
		}
	}


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
	SDL_SetWindowTitle(win->gWindow, std::to_string(win->FPS).c_str());
}

int main() {
	int error = 0;
	XY size = XY{ DEFUALT_WIDTH,DEFUALT_HEIGHT };
	std::ifstream in(config);
	if (!in) {
		std::cout << "Cannot open input file.\n";
	}
	std::string str;
	while (std::getline(in, str)) {
		str.erase(remove_if(str.begin(), str.end(), isspace), str.end());
		int i = str.find('=');
		std::string key = str.substr(0, i);
		std::string val = str.substr(i + 1, str.length());
		printf("key \"%s\" val \"%s\"\n", key.c_str(), val.c_str());
		if (!key.compare("IP")) {
			IP = val;
		}
		else if (!key.compare("PORT")) {
			PORT = val;
		}
		else {
			Registry[key] = std::stoi(val);
		}
	}
	win = new Window(size, RegInt("FULLSCREEN", 0) , &error);
	if (error) {
		std::cout << "error opening SDL window\n";
		return 1;
	}
	win->onExit = &OnExit;
	f = new Font(16, 16, "arial.ttf");
	client = Client();
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	draw = new Draw(size.x, size.y);
	int i;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE, &i);
	printf("size: %d\n", i);
	IM::Load();
	LoadWindowIcons();
	ContainerLoadWidgets();
	TL.Start();
	topLevel = new Container(new Container(widgets[0]), new Container(0));
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

