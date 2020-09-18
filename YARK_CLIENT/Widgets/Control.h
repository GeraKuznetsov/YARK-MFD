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
	void Toggle(std::string option, bool* ptr, XY pos);
	void SASOption(uint8_t thisMode, XY pos);
	void FloatPM(XY pos,std::string label, float* fl, float grad, int dig, bool sign);
public:
	virtual std::string GetTitle();
	Control();
	void Draw(XY pos, XY size);
};