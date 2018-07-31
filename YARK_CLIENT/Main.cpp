#include <iostream>
#include "Client\Client.h"
#include <iostream>
#include <string>
#include <vector>

#include "Engine\Window.h"
#include "Widgets\Console.h"
#include "Widgets\NavBall.h"

Client* client;
Window* win;
Font* f;
Cam* cam;

std::vector<Widget*> widgets;

#define DEFUALT_WIDTH 800
#define DEFUALT_HEIGHT 600

Console* console;
//NavBall* navball;

#include "Command.h"

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

	for (int i = 0; i < widgets.size(); i++) {
		widgets[i]->Tick(draw);
	}
}

void main() {
	int error = 0;
	XYi size = XYi{ DEFUALT_WIDTH,DEFUALT_HEIGHT };
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


	console = new Console(XY{ 0,0 }, XY{ DEFUALT_WIDTH,DEFUALT_HEIGHT }, "Console", f, win, &client, &widgets);
	widgets.push_back(console);
	console->command("config config.txt");
	win->Run(&Tick);

}

