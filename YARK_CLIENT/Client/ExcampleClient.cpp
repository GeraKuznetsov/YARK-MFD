#if false

/*
This is a simple script that checks for abort conditions and excecutes a very simple two step abort sequence
*/


#include <iostream>
#include <string>
#include "Client.h"

bool hadAbort = false;
int abortTime;
bool flightStarted = false;

void runExe() {
	Client* c = new Client("localhost", "9999");
	while (c->state == TCPCLIENT_CONNECTING); //wait for connection
	while (c->state == TCPCLIENT_CONNECTED) {
		if (c->statusPacket.inFlight) {//if there is a vessal in flight
			if (flightStarted) { //only check for abort conditions if we are flying

				if (c->vesselPacket.Prograde.Pitch < -10.f && c->vesselPacket.Alt < 50000 & !hadAbort) { //if our prograde vector's pitch is negative (aka we are falling),
																						   //and we are within the atmoshere, activate abort sequence
																						   // (-10 and 50000 are used instead of 0 and 70000 for margin of error)
					c->ControlPacket.MainControls = MC_ABORT; //stage abort
					abortTime = c->vesselPacket.MissionTime; //record abort time
					hadAbort = true;
					c->SendControls(); //send control packet
					c->ControlPacket.MainControls = 0; //reset abort
					std::cout << std::string(c->statusPacket.vessalName) << ": ABORTING\n";
				}
				if (hadAbort & (c->vesselPacket.MissionTime - abortTime) > 2 & c->vesselPacket.Alt < 10000 & c->vesselPacket.Prograde.Pitch < 70) { //step two of the abort sequence,
					std::cout << "step 2 of abort sequence\n";																	// , 2 seconds after step one,
					c->ControlPacket.ControlGroup = AG_1;	//action group 1																//stage the parachutes by activating action group 1
					c->SendControls(); //send control packet
					goto exit;
				}
			}
			else {
				if (c->vesselPacket.CurrentStage - c->vesselPacket.TotalStage != 0) { //if there was a staging, flight has started
					std::cout << std::string(c->statusPacket.vessalName) << ": flight started\n";
					flightStarted = true;
				}
			}
		}
	}
	std::cout << c->error << "\n";
	int foo;
	std::cin >> foo;
exit:;
}
#endif