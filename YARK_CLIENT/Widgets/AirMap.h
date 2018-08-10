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
	std::string name2;
	wayPoint start; 
	wayPoint stop;
	bool primary; //only used by map
};

class AirMap : public Widget {
	Target* target;
	GLuint kerbinMap;
	bool longLatFixed;
	glm::vec2 longLat;

	bool drag; //dragging
	XYi dragStart;
	glm::vec2 longLatDragStart;
	float head;

	float zoom;
	std::vector<Target> targets;
	void drawTarget(Target* t, Draw* draw, VesselPacket* VP, float zoom);
public:
	AirMap(WidgetStuff ws);
	void Tick(Draw* draw);
};