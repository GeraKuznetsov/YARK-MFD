#include <fstream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <thread>

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include "Client.h"

void Client::SendControls() {
	int	iResult = send(ConnectSocket, (char*)&ControlPacket, sizeof(ControlPacket), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}
	ControlPacket.ID++;
}

void Client::TCPClientRun(std::string IP, std::string PORT) {
#pragma region winsock stuff
	WSADATA wsaData;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	int iResult;

	// Validate the parameters

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		error = "WSAStartup failed";
		state = TCPCLIENT_FAILED;
		printf("WSAStartup failed with error: %d\n", iResult);
		return;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Resolve the server address and port
	iResult = getaddrinfo(IP.c_str(), PORT.c_str(), &hints, &result);
	if (iResult != 0) {
		error = "getaddrinfo failed";
		state = TCPCLIENT_FAILED;
		printf("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			error = "INVALID_SOCKET";
			state = TCPCLIENT_FAILED;
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		// Connect to server.
		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			error = "SOCKET_ERROR";
			//SERVER_LIST_PROMPT = SERVER_LIST_PROMPT_ERROR;
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		error = "Unable to connect to server!";
		state = TCPCLIENT_FAILED;
		printf("Unable to connect to server!\n");
		WSACleanup();
		return;
	}

	std::cout << "cleint in revieve loop\n";

	state = TCPCLIENT_CONNECTED;

#pragma endregion
	Running = true;
	do {
		iResult = recv(ConnectSocket, (char*)&hed, sizeof(Header), 0);
		if (iResult > 0) {
			if (hed.HEADER_0 == (char)0xDE && hed.HEADER_1 == (char)0xAD) {
				if (hed.packetType == (char)1) {
					recv(ConnectSocket, (char*)&statusPacket, sizeof(StatusPacket), 0);
				}
				else if (hed.packetType == (char)2) {
					recv(ConnectSocket, (char*)&vesselPacket, sizeof(VesselPacket), 0);
				}
			}
			else {
				std::cout << "malformed packet\n:";
				std::cout << (int)hed.HEADER_0 << " : " << (int)hed.HEADER_1 << "\n";
			}
		}
		else if (iResult == 0) {
			error = "recv failed";
			state = TCPCLIENT_FAILED;
			printf("Connection closed\n");
		}
		else {
			error = "con closed";
			state = TCPCLIENT_FAILED;
			printf("recv failed with error: %d\n", WSAGetLastError());
		}
	} while (iResult > 0 && Running);
	if (Running) {
		Shutdown();
	}
}

Client::Client(std::string IP, std::string PORT)
{
	memset((char*)&ControlPacket, 0, sizeof(ControlPacket));
	memset((char*)&vesselPacket, 0, sizeof(vesselPacket));
	memset((char*)&statusPacket, 0, sizeof(statusPacket));
	ControlPacket.HEADER_0 = 0xDE;
	ControlPacket.HEADER_1 = 0xAD;
	ControlPacket.ID = 0;
	recLoop = std::thread(&Client::TCPClientRun, this, IP, PORT);
	recLoop.detach();
}

void Client::Shutdown() {
	state = TCPCLIENT_FAILED;
	std::cout << "thread shutdown\n";
	Running = false;
	if (shutdown(ConnectSocket, SD_SEND) == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}
}
