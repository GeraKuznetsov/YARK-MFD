//dont show le console
//#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <iostream>
#include <iostream>
#include <string>
#include <vector>

#include "Client\Client.h"
#include "Engine\Window.h"
#include "Widgets\Console.h"
#include "Engine\Cam.h"
#include "Engine\Sound.h"

#include <sstream>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <thread>
#include <iostream>

Client* client;
Window* win;
Font* f;
Cam* cam;

std::vector<Widget*> widgets;

#define DEFUALT_WIDTH 800
#define DEFUALT_HEIGHT 600

Console* console;

#include "AltiMeter.h"

void Tick(float delta, Draw* draw) {
	VesselPacket VP = (client) ? (client)->vesselPacket : VesselPacket();

	cam->SetViewPort(0, 0, win->size.x, win->size.y);
	if (client) {
		if (client->state == TCPCLIENT_FAILED) {
			console->colorCur = 1;
			console->DispLine("client disconnected: " + client->error);
			delete client;
			client = NULL;
		}
		else if (client->state == TCPCLIENT_CONNECTED) {
			client->SendControls();
		}
	}

	RadioAltimeterTick(VP);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_SCISSOR_TEST);

	for (int i = 0; i < widgets.size(); i++) {
		glScissor(0, 0, win->size.x, win->size.y);
		widgets[i]->Tick(draw);
	}
	glScissor(0, 0, win->size.x, win->size.y);
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
	win->SetTargetFPS(60);
	f = new Font(16, 16, "C:\\Windows\\Fonts\\arial.ttf");

	cam = new Cam(0, 0, glm::vec3{ 0,0,0 });
	cam->fov = glm::radians(45.f);
	cam->orthro = true;

	console = new Console(WidgetStuff{ XY{ 0,0 }, XY{ size.x,size.y }, "Console", f, win, &client, new TextureLoader(),"console" });
	widgets.push_back(console);
	client = new Client(IP, port);
	console->command("config start.txt");
	OpenPlayer();
	for (int i = 0; i < WARNING_ALTADTUDES; i++) {
		warn_altitudes_sounds[i] = new Sound("Sound/" + std::to_string(warn_altitudes[i]) + ".wav");
	}
	SDL_Delay(1000);
	win->Run(&Tick);

}

