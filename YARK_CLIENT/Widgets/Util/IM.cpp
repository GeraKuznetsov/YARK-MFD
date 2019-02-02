#include "IM.h"

GLuint IM::rad0, IM::rad1, IM::T_UP, IM::T_DOWN, IM::led0, IM::led1, IM::seg_v, IM::seg_h;// , IM::push;
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
	if (!seg_v) {
		seg_v = loadTexture("Tex/7-seg-v.png", false);
	}
	if (!seg_h) {
		seg_h = loadTexture("Tex/7-seg-h.png", false);
	}

	//if (!push) {
	//	push = loadTexture("Tex/push.png", false);
	//}
}

#define SEG_STATE(x) (x) ? draw->SetDrawColor2D(0.98f,0.f,0.f) : draw->SetDrawColor2D(0.25f, 0.16f,0.16f);

#define v_space  14
#define h_space  14

#define h_move  2
#define v_move  2

#define width 20
#define height 20

void IM::SegDigit(XY pos, Window *win, Draw* draw, int dig) {
	draw->BindDraw2DShader();

	draw->BindTex2D(seg_h);
	SEG_STATE(dig == 2 || dig == 3 || dig == 5 || dig == 6 || dig == 7 || dig == 8 || dig == 9 || dig == 0);
	draw->DrawRect2D(pos.x + h_move, pos.y + v_space * 0, pos.x + width + h_move, pos.y + height + v_space * 0);
	SEG_STATE(dig == 2 || dig == 3 || dig == 4 || dig == 5 || dig == 6 || dig == 8 || dig == 9);
	draw->DrawRect2D(pos.x + h_move, pos.y + v_space * 1, pos.x + width + h_move, pos.y + height + v_space * 1);
	SEG_STATE(dig == 2 || dig == 3 || dig == 5 || dig == 6 || dig == 8 || dig == 0);
	draw->DrawRect2D(pos.x + h_move, pos.y + v_space * 2, pos.x + width + h_move, pos.y + height + v_space * 2);

	draw->BindTex2D(seg_v);
	SEG_STATE(dig == 4 || dig == 5 || dig == 6 || dig == 8 || dig == 9 || dig == 0);
	draw->DrawRect2D(pos.x, pos.y + v_move, pos.x + width, pos.y + height + v_move);
	SEG_STATE(dig == 2 || dig == 6 || dig == 8 || dig == 0);
	draw->DrawRect2D(pos.x, pos.y + v_space + v_move, pos.x + width, pos.y + height + v_space + v_move);

	SEG_STATE(dig == 1 || dig == 2 || dig == 3 || dig == 4 || dig == 7 || dig == 8 || dig == 9 || dig == 0);
	draw->DrawRect2D(pos.x + h_space, pos.y + v_move, pos.x + width + h_space, pos.y + height + v_move);
	SEG_STATE(dig == 1 || dig == 3 || dig == 4 || dig == 5 || dig == 6 || dig == 7 || dig == 8 || dig == 9 || dig == 0);
	draw->DrawRect2D(pos.x + h_space, pos.y + v_space + v_move, pos.x + width + h_space, pos.y + height + v_space + v_move);
}

void IM::SegInt(XY pos, Window *win, Draw* draw, int dig, int fig) {
	draw->BindDraw2DShader();
	if (dig < 0) {
		dig = -dig;
		SEG_STATE(1);
	}
	else {
		SEG_STATE(0);
	}
	draw->BindTex2D(seg_h);
	draw->DrawRect2D(pos.x + h_move - 20, pos.y + v_space * 1, pos.x + width + h_move - 20, pos.y + height + v_space * 1);
	int div = 10;
	for (int i = 0; i < fig; i++) {
		int mod = dig % div;
		SegDigit(pos + XY{ 36 - i * 20,0 }, win, draw, dig % div * 10 / div);
		div *= 10;
		dig -= mod;
	}
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