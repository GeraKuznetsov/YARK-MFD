#pragma once
#include "Widget.h"

class Control : public Widget {
	bool brakes, lights, gear, sas, rcs;
	GLuint push;
	XY lastPos;
	void Toggle(std::string option, bool* ptr, Draw* draw);
	void SASOption(Draw* draw, uint8_t thisMode, Client* c);
	void FloatPM(Draw* draw, float* f);
public:
	Control(WidgetStuff ws);
	void Tick(Draw* draw);
};