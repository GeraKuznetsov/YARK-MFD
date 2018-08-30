#include "Button.h"

bool IM::Button(XY pos, Window *win, Draw* draw, Font *f, std::string text) {
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(43.f / 256.f, 76.f / 256.f, 113.f / 256.f);
	draw->BindTex2D(0);
	XY size = XY{ f->GetTextWidth(text) + 15,20 };
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);
	draw->DrawString(f, text, pos.x + size.y / 2 - f->GetTextWidth(text) / 2, pos.y + 10);
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
