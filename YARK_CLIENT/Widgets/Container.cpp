#include "Container.h"
#include "../Engine/Texture.h"
#include "Util/IM.h"

Widget* optionsWidget ;
NavBall* navballWidget ;
AirMap* airMapWidget;
AtitudeIndicator* atiinWidget;
SoyuzNavBall* soyuzNavBallWidget;
OrbitDisplay* orbitDisplayWidget;
Control* controlWidget;
AirPlaneAutoPilot* airPlaneAutoPilotWidget;

void ContainerLoadWidgets() {
	 optionsWidget = new Widget();
	 navballWidget = new NavBall();
	 airMapWidget = new AirMap();
	 atiinWidget = new AtitudeIndicator();
	 soyuzNavBallWidget = new SoyuzNavBall();
	 orbitDisplayWidget = new OrbitDisplay();
	 controlWidget = new Control();
	 airPlaneAutoPilotWidget = new AirPlaneAutoPilot();
}

GLuint window_x, window_lr, window_ud;

void LoadWindowIcons() {
	window_x = loadTexture("Tex/x.png", false);
	window_lr = loadTexture("Tex/lr.png", false);
	window_ud = loadTexture("Tex/ud.png", false);
}

Container::Container(Container* l, Container* r){
	split = SPLIT::LEFT_RIGHT;
	left = l;
	right = r;
	division = 0.5;
}

Container::Container(Widget* w) {
	widget = w;
}

bool Container::Update(bool top, int posX, int posY, int width, int height) {
	if (split == NONE) {
		draw->SwitchShader(SHADER_2D);
		draw->BindTex2D(0);
		draw->SetDrawColor2D(1, 0, 0);
		draw->DrawRect2D(posX, posY, posX + width, posY + BAR_HEIGHT);
		if (IM::Button(XY{ posX + width - 24, posY + WID_BORDER }, XY{ 12, 12 }, window_x)) return true;
		if (IM::Button(XY{ posX + width - 24 - 24, posY + WID_BORDER }, XY{ 12, 12 }, window_lr)) {
			split = LEFT_RIGHT;
			division = 0.5;
			left = new Container(widget);
			right = new Container(0);
		}
		if (IM::Button(XY{ posX + width - 24 - 24 * 2, posY + WID_BORDER }, XY{ 12, 12 }, window_ud)) {
			split = UP_DOWN;
			division = 0.5;
			left = new Container(widget);
			right = new Container(0);
		}
		draw->SwitchShader(SHADER_TEXT);
		draw->SetTextColor(1, 1, 1);
		std::string title = widget ? widget->GetTitle() : "Open Widget";
		draw->DrawString(f, title, (width - f->GetTextWidth(title)) / 2 + posX, posY + 15);
		glEnable(GL_SCISSOR_TEST);
		glScissor(posX , win->size.y - posY - height, width , height - BAR_HEIGHT);
		if (widget) {
			widget->Draw(XY{ posX,posY+ BAR_HEIGHT }, XY{ width,height- BAR_HEIGHT });
		}
		else {
			draw->SwitchShader(SHADER_TEXT);
			draw->SetTextColor(1, 1, 1);
			float y = posY + 50;
			draw->DrawString(f, "Open widget:", posX + 15, y);
			if (IM::Button(XY{ posX + 15,y += 8 }, f, "Options")) widget = optionsWidget;
			if (IM::Button(XY{ posX + 15,y += 30 }, f, "Navball")) widget = navballWidget;
			if (IM::Button(XY{ posX + 15,y += 30 }, f, "AirMap")) widget = airMapWidget;
			if (IM::Button(XY{ posX + 15,y += 30 }, f, "Atitude Indicator")) widget = atiinWidget;
			if (IM::Button(XY{ posX + 15,y += 30 }, f, "Soyuz navball")) widget = soyuzNavBallWidget;
			if (IM::Button(XY{ posX + 15,y += 30 }, f, "Orbit Display")) widget = orbitDisplayWidget;
			if (IM::Button(XY{ posX + 15,y += 30 }, f, "Controller")) widget = controlWidget;
			if (IM::Button(XY{ posX + 15,y += 30 }, f, "AirPlane auto Pilot")) widget = airPlaneAutoPilotWidget;
		}
		glDisable(GL_SCISSOR_TEST);
	}
	else if (split == LEFT_RIGHT) {
		float divid = width * division;
		if (left->Update(false, posX, posY, divid - 2, height)) {
			split = right->split;
			division = right->division;
			widget = right->widget;
			Container* oldL = left, *oldR = right;
			left = right->left;
			right = right->right;
			delete oldL;
			delete oldR;
			return 0;
		}
		if (right->Update(false, posX + divid + 2, posY, width - (divid - 2) - WID_BORDER, height)) {
			split = left->split;
			division = left->division;
			widget = left->widget;
			Container *oldL = left, *oldR = right;
			right = left->right;
			left = left->left;
			delete oldL;
			delete oldR;
			return 0;
		}
		draw->SwitchShader(SHADER_2D);
		draw->BindTex2D(0);
		draw->SetDrawColor2D(0, 0, 1);
		int barX, barY, barX2, barY2;
		draw->DrawRect2D(barX = posX + divid - 2, barY = posY, barX2 = posX + divid + 2, barY2 = posY + height);
		if (win->MouseDown(SDL_BUTTON_LEFT)) {
			if (drag) {
				int newPos = win->MouseX();
				if (newPos < posX + 100) newPos = posX + 100;
				if (newPos > posX + width - 100) newPos = posX + width - 100;
				division = float(newPos - posX) / float(width);
			}
		}
		else if (win->MouseX() >= barX && win->MouseX() <= barX2 && win->MouseY() >= barY && win->MouseY() <= barY2) {
			drag = true;
		}
		else {
			drag = false;
		}
	}
	else if (split == UP_DOWN) {
		float divid = height * division;
		if (left->Update(false, posX, posY, width, divid - 2)) {
			split = right->split;
			division = right->division;
			widget = right->widget;
			Container* oldL = left, * oldR = right;
			left = right->left;
			right = right->right;
			delete oldL;
			delete oldR;
			return 0;
		}
		if (right->Update(false, posX, posY + divid + 2, width, height - (divid - 2) - WID_BORDER + 1)) {
			split = left->split;
			division = left->division;
			widget = left->widget;
			Container* oldL = left, * oldR = right;
			right = left->right;
			left = left->left;
			delete oldL;
			delete oldR;
			return 0;
		}
		draw->SwitchShader(SHADER_2D);
		draw->BindTex2D(0);
		draw->SetDrawColor2D(0, 0, 1);
		int barX, barY, barX2, barY2;
		draw->DrawRect2D(barX = posX, barY = posY + divid - 2, barX2 = posX + width, barY2 = posY + divid + 2);
		if (win->MouseDown(SDL_BUTTON_LEFT)) {
			if (drag) {
				drag = true;
				int newPos = win->MouseY();
				if (newPos < posY + 100) newPos = posY + 100;
				if (newPos > posY + height - 100) newPos = posY + height - 100;
				division = float(newPos - posY) / float(height);
			}
		}
		else if (win->MouseX() >= barX && win->MouseX() <= barX2 && win->MouseY() >= barY && win->MouseY() <= barY2) {
			drag = true;
		}
		else {
			drag = false;
		}
	}
	return false;
}
