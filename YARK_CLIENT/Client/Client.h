#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#include "Structs.h"
#define TCP_CONNECTING 0
#define TCP_FAILED 1
#define TCP_CONNECTED 2

class Client {
	SOCKET ConnectSocket;
	void Run(std::string IP, std::string PORT);
	bool ReadBytes(char *buffer, uint16_t* checkSum, int bytesToRead);
	void errBadPacket();
	std::thread recLoop;
	bool Running;
	volatile int state;
public:

	//IO packets
	StatusPacket Status;
	VesselPacket Vessel;
	ControlPacket Control;
	
	//Event callbacks
	void (*SPCallback)();
	void (*VPCallback)();
	
	char error[128];
	//connection Methods
	Client();
	void Connect(std::string IP, std::string PORT);
	bool Connected();
	void SendControls();
	void Shutdown();

    int GetState();
	void WaitForConnection();
};