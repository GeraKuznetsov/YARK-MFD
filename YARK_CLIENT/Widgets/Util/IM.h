#pragma once
#include "../../Engine/Window.h"
#include <string>

namespace IM {
	extern GLuint rad0, rad1, T_UP, T_DOWN, led0, led1;
		//, push;
	bool Button(XY pos, Window *win, Draw* draw, Font *f, std::string text);
	bool Button(XY pos, XY size, Window *win, Draw* draw, GLuint tex);
	bool Radio(XY pos, Window *win, Draw* draw, bool* ptr);
	void LED(XY pos, Window *win, Draw* draw, bool state);
	bool ToggleSwitch(XY pos, Window *win, Draw* draw,std::string text, bool* ptr);

	void Load();
}

#define TOGGLE_SIZE 64
