#pragma once
#include "../../Engine/Window.h"
#include <string>

namespace IM {
	extern GLuint rad0, rad1, T_UP, T_DOWN, led0, led1, seg_v, seg_h, push;
	//, push;
	void SegDigit(XY pos, Window *win, Draw* draw, int dig);
	void SegInt(XY pos, Window *win, Draw* draw, int dig, int fig, bool negS);
	bool PushButton(Draw* draw, Window *win, XY renderAt);
	bool Button(XY pos, Window *win, Draw* draw, Font *f, std::string text);
	bool Button(XY pos, XY size, Window *win, Draw* draw, GLuint tex);
	bool Radio(XY pos, Window *win, Draw* draw, bool* ptr);
	void LED(XY pos, Window *win, Draw* draw, bool state);
	bool ToggleSwitch(XY pos, Window *win, Draw* draw, std::string text, bool* ptr);

	void Load();
}

#define TOGGLE_SIZE 64
