#pragma once
#include "Widget.h"
#include "../Client/Client.h"
#include "../Engine/Texture.h"
#include "../Engine/Cam.h"
#include "../Engine/Window.h"
#include "../Engine/Draw.h"

class NavBall : public Widget {
	GLuint vao, vbo, shader, proj, model, rot;
	GLuint navballTex;
	GLuint chevron, PGTex, RGTex, RITex, ROTex, NTex, ANTex, TRGTex, TPGTex, MTex;

	Cam* cam;
	Client** client;
public:
	Window * win;
	NavBall(XY pos, XY size, std::string title, Font* font, Client** client, Cam* cam);
	void Tick(Draw* draw);
};