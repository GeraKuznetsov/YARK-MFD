#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#include "Struts.h"
#define TCPCLIENT_CONNECTING 0
#define TCPCLIENT_FAILED 1
#define TCPCLIENT_CONNECTED 2

class Client {
	SOCKET ConnectSocket;
	void TCPClientRun(std::string IP, std::string PORT);
	Header hed;
public:
	Status status;
	DataIn dataIn;
	ControlPacket CP;
	std::thread recLoop;
	bool Running;
	int state = TCPCLIENT_CONNECTING;
	std::string error;
	Client(std::string IP, std::string PORT);
	void SendControls();
	void Shutdown();
};