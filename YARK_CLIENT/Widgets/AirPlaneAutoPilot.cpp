#include "AirPlaneAutoPilot.h"
#include "Util/IM.h"
#include "../Reg.h"

AirPlaneAutoPilot::AirPlaneAutoPilot(WidgetStuff ws) : Control(ws) {
	RegInt("AP_MAX_AOA", 10);
	RegInt("AP_SPEED", 1000);
	RegInt("AP_ALT", 2000);
}

#define SIGN(x) ((x<0)?((float)-1):((float)1))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))

/*void AirPlaneAutoPilot::FloatPM(Draw* draw, float* fl, int dig, float grad) {
	if (IM::Button(pos + lastPos + XY{ 0,-7 }, win, draw, f, "-")) {
		(*fl) -= grad;
	}
	IM::SegInt(pos + lastPos + XY{ 45,-12 }, win, draw, int(*fl), dig);
	if (IM::Button(pos + lastPos + XY{ 100, -7 }, win, draw, f, "+")) {
		(*fl) += grad;
	}
	lastPos.y += 40;
}*/

void AirPlaneAutoPilot::Tick(Draw* draw) {
	WindowUpdate(draw);

	draw->BindDraw2DShader();
	draw->SetDrawColor2D(62.f / 256.f, 62.f / 256.f, 61.f / 256.f);
	draw->DrawRect2D(pos.x, pos.y, pos.x + size.x, pos.y + size.y);

	lastPos = XY{ 48,0 };

	Toggle("Autopilot", &enabled, draw);

	float maxAngle = Registry["AP_MAX_AOA"], alt = Registry["AP_ALT"], speed = Registry["AP_SPEED"];

	lastPos.y += 135;
	lastPos.x -= 50;
	FloatPM(draw, "Max AoA", &maxAngle, 1, 2, false);
	maxAngle = CLAMP(maxAngle, 0.f, 20.f);
	FloatPM(draw, "Target Altatude", &alt, 10, 5, false);
	alt = CLAMP(alt, 0, 20000);

	FloatPM(draw, "Target Airspeed", &speed, 5, 3, false);
	speed = CLAMP(speed, 0, 500);

	Registry["AP_MAX_AOA"] = maxAngle;
	Registry["AP_SPEED"] = speed;
	Registry["AP_ALT"] = alt;

	if (enabled) {
		if (client->Vessel.ID > lastID) { //If we got a new packet
			lastID = client->Vessel.ID;

			float deltaAlt = alt - client->Vessel.Alt;
			//printf("vessel: %f, taget: %f, d: %f\n", client->Vessel.Alt, alt, deltaAlt);

			Registry["SASS_HEADING"] = client->Vessel.Heading;
			Registry["SASS_ROLL"] = client->Vessel.Roll;

			float pitchTarg;

			if (abs(deltaAlt) < 200) {
				Registry["SASS_PITCH"] = max(sqrt(abs(deltaAlt / 200))  * maxAngle, 0.1f) * SIGN(deltaAlt);
			}
			//else if (abs(deltaAlt) < 200) {
			//	SASS.Pitch = (deltaAlt - 100) / 200 * maxAngle;
			//}
			else {
				Registry["SASS_PITCH"] = maxAngle * SIGN(deltaAlt);
				//printf("else mode dAlt: %f\n", deltaAlt);
			}



			double thisSpead = client->Vessel.Vsurf;
			double dSpeed = thisSpead - lastSpead; //Calculate change in speed between packets
			lastSpead = thisSpead;

			double thisTime = client->Vessel.MissionTime;
			double dTime = thisTime - lastTime; //Calulate change in time between packets
			lastTime = thisTime;

			double dSpeedPerSecond = dSpeed / dTime; //change in speed per second 

			double speedError = speed - thisSpead; //how much we need to change our speed

			double timeToGoal = (speedError) / dSpeedPerSecond; //how much time to reach the goal speed

			double dir = SIGN(dSpeedPerSecond * speedError);

			printf("dSpeedPerSecond: %f, speedError: %f", dSpeedPerSecond, speedError);

			if (dSpeedPerSecond < 0) {
				if (speedError < 0) {
					//use curve
				}
				else {
					//speed up
					printf("speed up\n");
					THROTTLE = 1000;
				}
			}
			else {
				if (speedError < 0) {
					//slow down
					printf("slow down\n");
					THROTTLE = 0;
				}
				else {
					//use curve
				}
			}
		}
		printf("foooo\n");
		client->Control.Throttle = THROTTLE;
		client->Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_OVERIDE);

	}
	else {
		client->Control.SetControlerMode(CONTROLLER_THROTTLE, AXIS_EXT_NZ);
	}
	//if (win->HasJoyStick()) {
	//Option("Fly-By-Wire", "ENABLE_FLYBYWIRE", draw);
	//Option("Rocket Mode", "FLYBYWIRE_ROCKETMODE", draw);
	//Option("Smart Attitude control", "FORCE_SASS", draw);
	//}
}