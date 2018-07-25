#include "Widget.h"

#define BAR_HEIGHT 15

Widget::Widget(XY pos, XY size, std::string title,Font* f) {
	this->pos = pos;
	this->size = size;
	this->title = title;
	this->f = f;
	pos.y += BAR_HEIGHT;
	size.y -= BAR_HEIGHT;
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