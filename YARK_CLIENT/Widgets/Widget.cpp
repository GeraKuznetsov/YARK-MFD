#include "Widget.h"

#define BAR_HEIGHT 15

Widget::Widget(XY pos, XY size, std::string title, Font* f) {
	Resize(pos, size);
	this->f = f;
	this->title = title;
}
void Widget::Resize(XY pos, XY size) {
	this->pos = pos;
	this->size = size;
	this->pos.y += BAR_HEIGHT;
	this->size.y -= BAR_HEIGHT;
}

void Widget::Tick(Draw* draw) {
	RenderWindow(draw);
}

void Widget::RenderWindow(Draw* draw) {
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0.2, 0.2, 0.5, 0.5);
	draw->BindTex2D(0);
	draw->DrawRect2D(pos.x, pos.y - BAR_HEIGHT, pos.x + size.x, pos.y + size.y);
	draw->SetDrawColor2D(0.2, 0.2, 0.8, 1);
	draw->DrawRect2D(pos.x, pos.y - BAR_HEIGHT, pos.x + size.x, pos.y);
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, title, pos.x, pos.y);
}