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
	std::thread recLoop;
	bool Running;
public:
	StatusPacket statusPacket;
	VesselPacket vesselPacket;
	ControlPacket controlPacket;
	int state = TCPCLIENT_CONNECTING;
	std::string error;
	//connection Methods
	Client();
	void Connect(std::string IP, std::string PORT);
	bool Connected();
	void SendControls();
	void Shutdown();

	//Helper Methods
	void InputRot(float pitch, float yaw, float roll);
	void InputTran(float tx, float ty, float tz);
	void InputThrottle(float throttle);
	void ReSetSASHoldVector();
	void SetSASHoldVector(float pitch, float heading, float roll);
	void SetControlerMode(int controler, int mode);
	bool GetActionGroup(int group);
	void SetActionGroup(int group, bool s);
	bool GetMainControl(int control);
	void SetMainControl(int control, bool s);
};