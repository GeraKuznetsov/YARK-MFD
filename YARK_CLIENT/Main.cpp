#include <iostream>
#include "Client\Client.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>

#include "Engine\Window.h" //include last due to conflicts with "DrawText"
#include "Widgets\Console.h"
#include "Widgets\NavBall.h"

Client* client;
Window* win;
Font* f;
Cam* cam;

#define WIDTH 800
#define HEIGHT 600

Console* console;
NavBall* navball;

void Tick(float delta, Draw* draw) {
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

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	console->Tick(draw);
	navball->Tick(draw);
}

std::vector<std::string> split(const std::string s, char delim) {
	std::istringstream iss(s);
	std::vector<std::string> tokens{ std::istream_iterator<std::string>{iss},
		std::istream_iterator<std::string>{} };
	return tokens;
}

void commandExe(std::string com) {
	console->colorCur = 0;
	console->DispLine(">" + com);
	std::vector<std::string> elems = split(com, ' ');
	if (!elems[0].compare("connect")) {
		console->DispLine("connecting...");
		client = new Client(elems[1], elems[2]);
		while (client->state == TCPCLIENT_CONNECTING) {

		}
		if (client->state == TCPCLIENT_CONNECTED) {
			console->colorCur = 2;
			console->DispLine("conneced");
		}
		else if (client->state == TCPCLIENT_FAILED) {
			console->colorCur = 1;
			console->DispLine("error: " + client->error);
			delete client;
			client = NULL;
		}
	}
}

void command(std::string com) {
	std::thread t(&commandExe, com);
	t.detach();
}

void main() {
	int error = 0;
	XYi size = XYi{ 720,720 };
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
	
	console = new Console(win, XY{ 0,0 });
	command("connect localhost 9999");
	navball = new NavBall(XY{ 50,50 }, XY{ 500,500 }, "NavBall", f, &client, cam);
	navball->win = win;
	win->Run(&Tick);

}

