#pragma once
#include "Widget.h"

class Control : public Widget {
	bool mc[7];
	bool ag[10];
	GLuint  sliderKnob;
	bool armAbort, armStage;
	VesselPacket oldVessel;
	bool drag;
protected:
	XY lastPos;
	void Toggle(std::string option, bool* ptr, Draw* draw);
	void SASOption(Draw* draw, uint8_t thisMode, Client* c);
	void FloatPM(Draw* draw, std::string label, float* fl, float grad, int dig, bool sign);
public:
	Control(WidgetStuff ws);
	void Tick(Draw* draw);
};