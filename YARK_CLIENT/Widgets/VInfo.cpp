#include "VInfo.h"
#include "../Reg.h"

VInfo::VInfo(WidgetStuff ws) : Widget(ws) {

}

void VInfo::Tick(Draw* draw) {
	WindowUpdate(draw);
	draw->BindTextShader();
	if (*client) {
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "In Flight: " + std::to_string((*client)->statusPacket.inFlight), pos.x+2, pos.y+12);
		draw->DrawString(f, "Vessel Name: " + std::string((*client)->statusPacket.vessalName), pos.x+2, pos.y + 27);
	}
	else {
		draw->SetTextColor(1, 1, 1);
		draw->DrawString(f, "Client Not Connected", pos.x + 2, pos.y + 12);
	}
}