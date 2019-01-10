#include "IM.h"

GLuint IM::rad0, IM::rad1, IM::T_UP, IM::T_DOWN, IM::led0, IM::led1;// , IM::push;
//Font* IM::f;

void IM::Load() {
	if (!rad0) {
		rad0 = loadTexture("Tex/radio-off.png", false);
	}
	if (!rad1) {
		rad1 = loadTexture("Tex/radio-on.png", false);
	}
	if (!T_UP) {
		T_UP = loadTexture("Tex/t-up.png", false);
	}
	if (!T_DOWN) {
		T_DOWN = loadTexture("Tex/t-down.png", false);
	}
	if (!led0) {
		led0 = loadTexture("Tex/led0.png", false);
	}
	if (!led1) {
		led1 = loadTexture("Tex/led1.png", false);
	}
	//if (!push) {
	//	push = loadTexture("Tex/push.png", false);
	//}
}

bool IM::Button(XY pos, Window *win, Draw* draw, Font *f, std::string text) {
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(43.f / 256.f, 76.f / 256.f, 113.f / 256.f);
	draw->BindTex2D(0);
	int textWidth = f->GetTextWidth(text);
	XY size = XY{ textWidth + 15,20 };
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
	draw->BindTextShader();
	draw->DrawString(f, text, pos.x + (size.x / 2) - (textWidth / 2), pos.y + 15);
	if (win->MouseX() > pos.x && win->MouseY() > pos.y && win->MouseX() < pos.x + size.x && win->MouseY() < pos.y + size.y) {
		return win->MouseClicked(SDL_BUTTON_LEFT);
	}
	return false;
}
bool IM::Button(XY pos, XY size, Window *win, Draw* draw, GLuint tex) {
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(1, 1, 1);
	draw->BindTex2D(tex);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
	draw->BindTex2D();
	if (win->MouseX() > pos.x && win->MouseY() > pos.y && win->MouseX() < pos.x + size.x && win->MouseY() < pos.y + size.y) {
		return win->MouseClicked(SDL_BUTTON_LEFT);
	}
	return false;
}

bool IM::Radio(XY pos, Window *win, Draw* draw, bool* ptr) {
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(1, 1, 1);
	draw->BindTex2D(*ptr ? IM::rad1 : IM::rad0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + 16, pos.y + 16);
	draw->BindTex2D();
	if (win->MouseX() > pos.x && win->MouseY() > pos.y && win->MouseX() < pos.x + 16 && win->MouseY() < pos.y + 16) {
		if (win->MouseClicked(SDL_BUTTON_LEFT)) {
			*ptr = !*ptr;
			return true;
		}
	}
	return false;
}

#define YOFFSET 32
#define XOFFSET 2

extern Font* f;

void IM::LED(XY pos, Window *win, Draw* draw, bool state) {
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(1, 1, 1);
	draw->BindTex2D(state ? led1 : led0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + 32, pos.y + 32);
}

bool IM::ToggleSwitch(XY pos, Window *win, Draw* draw, std::string text, bool* ptr) {
	LED(XY{ pos.x + 16, pos.y }, win, draw, *ptr);
	draw->BindTex2D(*ptr ? IM::T_UP : IM::T_DOWN);
	int offset = *ptr ? YOFFSET : YOFFSET + 6;
	draw->DrawRect2D(pos.x + XOFFSET, pos.y + offset, pos.x + TOGGLE_SIZE + XOFFSET, pos.y + TOGGLE_SIZE + offset);
	draw->BindTex2D();
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, text, pos.x + TOGGLE_SIZE / 2 - f->GetTextWidth(text) / 2, pos.y + TOGGLE_SIZE + 54);
	if (win->MouseX() > pos.x + XOFFSET && win->MouseY() > pos.y + offset && win->MouseX() < pos.x + XOFFSET + TOGGLE_SIZE && win->MouseY() < pos.y + offset + TOGGLE_SIZE) {
		if (win->MouseClicked(SDL_BUTTON_LEFT)) {
			*ptr = !*ptr;
			return true;
		}
	}
	return false;
}