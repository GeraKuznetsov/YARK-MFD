#pragma once

#if _WIN32
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#else
#include <sys/socket.h>
#endif
#include <thread>
#include "Structs.h"
#define TCP_CONNECTING 0
#define TCP_FAILED 1
#define TCP_CONNECTED 2
#include <vector>
#include <glm/vec3.hpp>
#include <cstring>

struct OrbitPlanStruct {
	std::vector<OrbitData> CurrentOrbitPatches;
	int ManPatchNum;
	std::vector<OrbitData> PlannedOrbitPatches;
	OrbitData TargetOrbit;
	std::string TargetName;
	std::vector<ManData> Mans;
	ClosestAprouchData CAD;
};

class Client {
	struct {
		Header header;
		uint8_t mode; //0=set //1=new //2=delete
		uint8_t manID;
		double UT;
		float X, Y, Z;
	} ManChangePacket;

#ifdef _WIN32
	SOCKET ConnectSocket;
#else
	int ConnectSocket;
#endif
	void Run(std::string IP, std::string PORT);
	bool ReadBytes(char *buffer, uint16_t* checkSum, int bytesToRead);
	void errBadPacket();
	std::thread recLoop;
	bool Running;
	volatile int state;
	void Send(char *buff, int length);
public:

	//IO packets
	StatusPacket Status;
	VesselPacket Vessel;
	ControlPacket Control;
	OrbitPlanStruct OrbitPlan;

	//Event callbacks
	void(*SPCallback)();
	void(*VPCallback)();

	char error[128];
	//connection Methods
	Client();
	void Connect(std::string IP, std::string PORT);
	bool Connected();
	void SendManChange(uint8_t mode, uint8_t ID, float UT, glm::vec3 vector);
	void SendControls();
	void Shutdown();

	int GetState();
	void WaitForConnection();
};
