#pragma once
#include "Widget.h"
#include "../Client/Client.h"
#include "../Engine/Texture.h"
#include "../Engine/Cam.h"
#include "../Engine/Window.h"
#include "../Engine/Draw.h"

class NavBall : public Widget {
	GLuint vao, vbo, shader, proj, model, rot,roll;
	Texture* navballTex;
	Cam* cam;
	Window* win;
	Client** client;
	Font* f;
public:
	NavBall(Client** client, XY pos, Cam* cam, Window* win, Font* font); //REMOVE WINDOW
	void Tick(Draw* draw);
};