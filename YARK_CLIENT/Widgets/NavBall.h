#pragma once
#include "Widget.h"
#include "../Client/Client.h"
#include "../Engine/Texture.h"
#include "../Engine/Cam.h"
#include "../Engine/Window.h"
#include "../Engine/Draw.h"

class NavBall : public Widget {
	GLuint vao, vbo, shader, proj, model, rot;
	Texture* navballTex;
	Texture* chevron;
	
	Cam* cam;
	Client** client;
public:
	NavBall(XY pos, XY size, std::string title, Font* font, Client** client,Cam* cam);
	void Tick(Draw* draw);
};