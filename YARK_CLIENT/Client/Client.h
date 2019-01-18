#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#include "Structs.h"
#define TCPCLIENT_CONNECTING 0
#define TCPCLIENT_FAILED 1
#define TCPCLIENT_CONNECTED 2

class Client {
	SOCKET ConnectSocket;
	void TCPClientRun(std::string IP, std::string PORT);
	bool ReadBytes(char *buffer, uint16_t* checkSum, int bytesToRead);
public:
	StatusPacket statusPacket;
	VesselPacket vesselPacket;
	ControlPacket ControlPacket;
	std::thread recLoop;
	bool Running;
	int state = TCPCLIENT_CONNECTING;
	std::string error;
	//connection Methods
	Client();
	void Connect(std::string IP, std::string PORT);
	bool Connected();
	void SendControls();
	void Shutdown();
	//Helper Methods
	bool GetMainControls(int control);
	void SetMainControls(int control, bool s);
};