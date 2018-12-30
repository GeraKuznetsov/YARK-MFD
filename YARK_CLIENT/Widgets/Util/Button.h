#pragma once
#include "../../Engine/Window.h"
#include <string>

namespace IM {
	extern GLuint rad0, rad1;

	bool Button(XY pos, Window *win, Draw* draw, Font *f, std::string text);
	bool Button(XY pos, XY size, Window *win, Draw* draw, GLuint tex);
	bool Radio(XY pos, Window *win, Draw* draw, bool* ptr);
}