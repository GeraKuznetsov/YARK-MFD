#include "LaunchAss.h"
#include "Util/IM.h"
#include "../Reg.h"
LaunchAss::LaunchAss(WidgetStuff ws) : Widget(ws) {

}

float target_heading = 90.f;

//MOVE AWAY FROM GLOBAL TO CLASS
float straight_alt = 300;
float targ_alt = 70 * 1000;

#define LA_STANDBY 0
#define LA_STRAIGHT 1
#define LA_TURN 2

int state = LA_STANDBY;

void LaunchAss::Tick(Draw* draw) {
	WindowUpdate(draw);
	draw->BindDraw2DShader();
	draw->SetDrawColor2D(0.5, 0.5, 0.5);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	draw->SetDrawColor2D(0, 0, 0);
	draw->DrawRect2D(pos.x + 5, pos.y + 5, pos.x + size.x - 5, pos.y + size.y - 35);

	VesselPacket VP = client->Vessel;
	ControlPacket *CP = &(client->Control);
	//CP = (*client)->ControlPacket;

	switch (state) {
	case LA_STANDBY:
	{
		if (IM::Button(XY{ pos.x + 5,  pos.y + size.y - 30 }, win, draw, f, "Launch Rocket")) {
			state = LA_STRAIGHT;
			CP->MainControls = 0;
		}
	} break;
	case LA_STRAIGHT:
	{
		//CP->SASMode = SAS_HOLD_VECTOR;
		//Registry["ENABLE_FLYBYWIRE"] = 0;
		//Registry["FLYBYWIRE_SMART"] = 1;
		CP->MainControls = MC_SAS;
		CP->targetHeading = target_heading;
		CP->targetRoll = VP.Roll;
		float throttle = 100;
		if (VP.Vsurf > 200.f) {
			throttle -= (VP.Vsurf - 200); //lets not go too fast
		}
		if (throttle < 5) {
			throttle = 5;
		}
		CP->Throttle = int(throttle * 10);

		CP->targetPitch = 90.f - (VP.Alt / 10000.f * 10.f);

		if (VP.Alt > 10000) {
			state = LA_TURN;
		}
		printf("%f : %f\n", VP.LiquidFuelTotS, VP.LiquidFuelS);
	} break;
	case LA_TURN:
		printf("todo");
		break;
	}

}
