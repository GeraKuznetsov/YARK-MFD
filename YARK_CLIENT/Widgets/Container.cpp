#include "Container.h"
#include "../Wrap/Texture.h"
#include "Util/IM.h"
#include "Fuel.h"
#include "Grapher.h"
#include "GroundTrack.h"
#include <string>
std::vector<Widget*> widgets;


void Container::Free() {
	if (left) left->Free();
	if (right) right->Free();
	delete left;
	delete right;
}

Container::Container(std::string data) {
	printf("parsing: %s\n", data.c_str());
	int first = data.find(",");
	split = (SPLIT)std::stoi(data.substr(0, first));
	if (split == SPLIT::NONE) {
		printf("\"%s\"\n", data.substr(first, data.length() - first).c_str());
		int nextInt = std::stoi(data.substr(first + 1, data.length() - first - 1));
		widget = widgets[nextInt];
	}
	else {
		int next = data.find(",", first + 1);
		printf("%d \"%s\"\n", next, data.substr(first + 1, next - first - 1).c_str());
		int nextInt = std::stoi(data.substr(first + 1, next - first - 1));
		division = float(nextInt) / 10000.f;
		int last = next + 1;
		int count = 0;
		do {
			count += data[last] == '{';
			count -= data[last] == '}';
			last++;
		} while (count);
		left = new Container(data.substr(next + 2, last - next - 3));
		right = new Container(data.substr(last + 1, data.size() - last - 2));
	}
}

std::string Container::Save() {
	std::string out = "{";
	out += std::to_string((int)split) + ",";
	if (split == SPLIT::NONE) {
		int num = 666;
		for (int i = 0; i < widgets.size(); i++) {
			if (widget == widgets[i]) {
				num = i;
			}
		}
		out += std::to_string(num);
	}
	else {
		out += std::to_string((int)(division * 10000)) + ",";
		out += left->Save();
		out += right->Save();
	}
	return out + "}";
}

void ContainerLoadWidgets() {
	widgets.push_back(new Widget());
	widgets.push_back(new NavBall());
	widgets.push_back(new AirMap());
	widgets.push_back(new AtitudeIndicator());
	widgets.push_back(new SoyuzNavBall());
	widgets.push_back(new OrbitDisplay());
	widgets.push_back(new GroundTrack());
	widgets.push_back(new Control());
	widgets.push_back(new AirPlaneAutoPilot());
	widgets.push_back(new FuelDisplay());
	widgets.push_back(new Dock());
	for (int i = 0; i < 5; i++) {
		widgets.push_back(new Grapher(i));
	}
}

GLuint window_x, window_lr, window_ud, swap_h, swap_v;

void LoadWindowIcons() {
	window_x = loadTexture("Tex/x.png", false);
	window_lr = loadTexture("Tex/lr.png", false);
	window_ud = loadTexture("Tex/ud.png", false);
	swap_v = loadTexture("Tex/swap_v.png", false);
	swap_h = loadTexture("Tex/swap_h.png", false);
}

Container::Container(Container* l, Container* r) {
	split = SPLIT::LEFT_RIGHT;
	left = l;
	right = r;
	division = 0.5;
}

Container::Container(Widget* w) {
	widget = w;
}

bool Container::Update(bool top, int posX, int posY, int width, int height) {
	if (split == SPLIT::NONE) {
		draw->SwitchShader(SHADER_2D);
		draw->BindTex2D(0);
		draw->SetDrawColor2D(1, 0, 0);
		draw->DrawRect2D(posX, posY, posX + width, posY + BAR_HEIGHT);
		if (IM::Button(XY{ posX + width - 24, posY + WID_BORDER }, XY{ 12, 12 }, window_x)) return true;
		if (IM::Button(XY{ posX + width - 24 - 24, posY + WID_BORDER }, XY{ 12, 12 }, window_lr)) {
			split = SPLIT::LEFT_RIGHT;
			division = 0.5;
			left = new Container(widget);
			right = new Container(0);
		}
		if (IM::Button(XY{ posX + width - 24 - 24 * 2, posY + WID_BORDER }, XY{ 12, 12 }, window_ud)) {
			split = SPLIT::UP_DOWN;
			division = 0.5;
			left = new Container(widget);
			right = new Container(0);
		}
		draw->SwitchShader(SHADER_TEXT);
		draw->SetTextColor(1, 1, 1);
		std::string title = widget ? widget->GetTitle() : "Open Widget";
		draw->DrawString(f, title, (width - f->GetTextWidth(title)) / 2 + posX, posY + 15);
		glEnable(GL_SCISSOR_TEST);
		glScissor(posX, win->size.y - posY - height, width, height - BAR_HEIGHT);
		if (widget) {
			widget->Draw(XY{ posX,posY + BAR_HEIGHT }, XY{ width,height - BAR_HEIGHT });
		}
		else {
			draw->SwitchShader(SHADER_TEXT);
			draw->SetTextColor(1, 1, 1);
			float y = 50;
			draw->DrawString(f, "Open widget:", posX + 15, y);
			int x = posX + 15;
			y += 8 - 30;
			for (int i = 0; i < widgets.size(); i++) {
				if (IM::Button(XY{x, posY +( y += 30) }, f, widgets[i]->GetTitle())) widget = widgets[i];
				if (y >  height - 50) {
					y = 50 + 8 - 30;
					x += 200;
				}
			}
		}
		glDisable(GL_SCISSOR_TEST);
	}
	else if (split == SPLIT::LEFT_RIGHT) {
		float divid = width * division;
		if (left->Update(false, posX, posY, divid - 2, height)) {
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
		if (right->Update(false, posX + divid + 2, posY, width - (divid - 2) - WID_BORDER, height)) {
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
		draw->DrawRect2D(barX = posX + divid - 2, barY = posY, barX2 = posX + divid + 2, barY2 = posY + height);
		if (IM::Button(XY{ barX + 6, barY + 2 }, XY{ 16,16 }, swap_h)) {
			Container* temp = left;
			left = right;
			right = temp;
		}
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
	else if (split == SPLIT::UP_DOWN) {
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
		if (IM::Button(XY{ barX + 6 , barY + 4 }, XY{ 16,16 }, swap_v)) {
			Container* temp = left;
			left = right;
			right = temp;
		}
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
