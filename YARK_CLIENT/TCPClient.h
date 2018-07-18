#pragma once

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <thread>
#define TCPCLIENT_CONNECTING 0
#define TCPCLIENT_FAILED 1
#define TCPCLIENT_CONNECTED 2

class TCPClient {
	SOCKET ConnectSocket;
	void TCPClientRun(std::string IP, std::string PORT, void(*callback_proccess)(char[], TCPClient*));
public:
	std::thread recLoop;
	bool Running;
	int state = TCPCLIENT_CONNECTING;
	std::string error;
	int ClientSend(char sendbuff[]);
	TCPClient(std::string IP, std::string PORT, void(*callback_proccess)(char[], TCPClient*));
	void Shutdown();
};