#pragma once
#include "Widget.h"
#include "../Client/Client.h"
#include "../Engine/Texture.h"
#include "../Engine/Window.h"
#include "../Engine/Draw.h"

#define BORDER_SCALE 1.05f

struct SphereDraw {
	GLuint vao, vbo, shader, proj, modelUnif, rotUnif;
	SphereDraw();
	void DrawSphere(Draw *draw, GLuint texture, glm::mat4 modelMat, glm::mat4 rotMat);
};

class NavBall : public Widget {
	GLuint navballTex;
	GLuint chevron, PGTex, RGTex, RITex, ROTex, NTex, ANTex, TRGTex, TPGTex, MTex, SASSTex;
public:
	static SphereDraw *SD;
	NavBall(WidgetStuff ws);
	void LoadNavBallTextures();
	void Tick(Draw* draw);
};