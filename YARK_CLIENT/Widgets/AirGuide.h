#pragma once
#include "Widget.h"
#include "../Client/Client.h"
#include "../Engine/Texture.h"
#include "../Engine/Draw.h"
#include <vector>

struct wayPoint {
	glm::vec2 coord;
	float alt;
};

struct Target {
	std::string name;
	wayPoint start;
	wayPoint stop;
};

class AirGuide : public Widget {
	Client** client;
	Target* target;
	GLuint kerbinMap;
	std::vector<Target> targets;
	void drawTarget(Target* t, Draw* draw, VesselPacket* VP, float zoom);
public:
	AirGuide(XY pos, XY size, std::string title, Font* font, Client** client);
	void Tick(Draw* draw);
};