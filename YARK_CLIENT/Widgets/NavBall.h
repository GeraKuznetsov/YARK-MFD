#pragma once
#include "Widget.h"
#include "../Client/Client.h"
#include "../Engine/Texture.h"
#include "../Engine/Window.h"
#include "../Engine/Draw.h"

class NavBall : public Widget {
	GLuint vao, vbo, shader, proj, modelUnif, rotUnif;
	GLuint navballTex;
	GLuint chevron, PGTex, RGTex, RITex, ROTex, NTex, ANTex, TRGTex, TPGTex, MTex;

public:
	NavBall(WidgetStuff ws);
	void Tick(Draw* draw);
};