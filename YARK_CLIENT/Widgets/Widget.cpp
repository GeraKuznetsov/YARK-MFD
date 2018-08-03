#include "Widget.h"

#define BAR_HEIGHT 15
#define BORDER 4

Widget::Widget(XY pos, XY size, std::string title, Font* f) {
	Resize(pos, size);
	this->f = f;
	this->title = title;
}
void Widget::Resize(XY pos, XY size) {
	this->pos = pos;
	this->size = size;
	this->pos.y += BAR_HEIGHT;
	this->size.y -= BAR_HEIGHT - BORDER;
	this->pos.x += BORDER;
	this->size.x -= BORDER * 2;
}

void Widget::Tick(Draw* draw) {
	RenderWindow(draw);
}

void Widget::RenderWindow(Draw* draw) {
	draw->BindDraw2DShader();
	draw->BindTex2D(0);
	draw->SetDrawColor2D(0, 0, 1);
	draw->DrawRect2D(pos.x - BORDER, pos.y - BAR_HEIGHT, pos.x + size.x + BORDER, pos.y + size.y + BORDER);
	
	draw->BindTextShader();
	draw->SetTextColor(1, 1, 1);
	draw->DrawString(f, title, pos.x, pos.y);
}