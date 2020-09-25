#pragma once
#include "../../Wrap/Window.h"
#include <string>

#define TEXT_ALLOW_NUM 1
#define TEXT_ALLOW_CHAR 2
#define TEXT_ALLOW_OTHER 4
#define TEXT_ALLOW_ALL 7

struct TextBox {
	std::string* text;
	bool type = 0;
	int pos = 0;
	int flag;
	TextBox(std::string* t, int f){
		text = t;
		flag = f;
	}
};

namespace IM {
	extern GLuint rad0, rad1, T_UP, T_DOWN, led0, led1, seg_v, seg_h, push;
	//, push;
	void TextInput(XY pos, int width, Font* f, TextBox* tb, std::string prompt = "");
	void SegDigit(XY pos, int dig);
	void SegInt(XY pos, int dig, int fig, bool negS);
	bool PushButton(XY renderAt);
	bool Button(XY pos, Font* f, std::string text);
	bool Button(XY pos, XY size, GLuint tex);
	bool Radio(XY pos, bool* ptr, Font* f, std::string text);
	void LED(XY pos, bool state);
	bool ToggleSwitch(XY pos, std::string text, bool* ptr);

	void Load();
}

#define TOGGLE_SIZE 64
