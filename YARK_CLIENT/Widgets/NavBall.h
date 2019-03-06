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

struct renderCoords {
	float x, y, alpha;
};

class NavBall : public Widget {
	GLuint navballTex;
	GLuint chevron;
	//Font* font;
	void DrawRect(int x, int y, int width, int height, Draw* draw);
public:
	renderCoords calcRenderCoords(NavHeading NH, VesselPacket *VP, glm::mat4 *modelMat);
	void  renderNavHeading(NavHeading NH, VesselPacket *VP, Draw* draw, glm::mat4 *modelMat, GLuint Tex);
	static SphereDraw *SD;
	NavBall(WidgetStuff ws);
	void LoadNavBallTextures();
	void Tick(Draw* draw);
};