#pragma once
#include "../../Engine/Window.h"
#include <string>

namespace IM {
	bool Button(XY pos, Window *win, Draw* draw, Font *f, std::string text);
	bool Button(XY pos, XY size, Window *win, Draw* draw, GLuint tex);
}