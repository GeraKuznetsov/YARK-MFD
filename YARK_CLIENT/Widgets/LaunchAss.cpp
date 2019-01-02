#include "LaunchAss.h"
#include "../Reg.h"
LaunchAss::LaunchAss(WidgetStuff ws) : Widget(ws) {

}



void LaunchAss::Tick(Draw* draw) {
	WindowUpdate(draw);
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	VesselPacket VP = (*client) ? (*client)->vesselPacket : VesselPacket();


}
