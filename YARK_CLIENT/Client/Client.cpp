#include <assert.h> 
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

#define RECV_BUFF_SIZE (1024*16)

uint16_t checksum(uint8_t *buffer, int length) {
	uint16_t acc = 0;
	for (int i = 0; i < length; i++) {
		acc += buffer[i];
	}
	return (uint16_t)acc;
}

void Client::SendControls() { //use async ?
	controlPacket.header.checksum = checksum((uint8_t*)&controlPacket.ID, sizeof(controlPacket) - sizeof(Header));
	int	iResult = send(ConnectSocket, (char*)&controlPacket, sizeof(controlPacket), 0);
	if (iResult == SOCKET_ERROR) {
		int errorN = WSAGetLastError();
		if (errorN != WSAEWOULDBLOCK) {
			printf("send failed with error: %d\n", errorN);
			closesocket(ConnectSocket);
			WSACleanup();
		}
	}
	controlPacket.ID++;
}

bool Client::ReadBytes(char *buffer, uint16_t *checkSum, int bytesToRead) {
	int bytesRead = 0;
	while (bytesRead < bytesToRead) {
		int result = recv(ConnectSocket, buffer + bytesRead, bytesToRead - bytesRead, 0);
		if (result > 0) {
			bytesRead += result;
		}
		else if (result == 0) {
			error = "recv failed";
			state = TCPCLIENT_FAILED;
			printf("Connection closed\n");
			return false;
		}
		else {
			int errorN = WSAGetLastError();
			if (errorN != WSAEWOULDBLOCK) {
				error = "con closed";
				state = TCPCLIENT_FAILED;
				printf("recv failed with error: %d\n", errorN);
				return false;
			}
		}
	}
	if (checkSum) {
		*checkSum = checksum((uint8_t*)buffer, bytesToRead);
	}
	return true;
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

	/*
	u_long mode = 1;
	iResult = ioctlsocket(ConnectSocket, FIONBIO, &mode);
	if (iResult != NO_ERROR) {
		error = "oof";
		state = TCPCLIENT_FAILED;
		printf("ioctlsocket failed with error: %ld\n", iResult);
		WSACleanup();
		return;
	}*/

	std::cout << "cleint in revieve loop\n";

	state = TCPCLIENT_CONNECTED;

#pragma endregion
	Running = true;
	StatusPacket sP;
	VesselPacket vP;

	Header header;

	uint16_t checkSumHeader;
	uint16_t checksumCalced;

	while (Running && state == TCPCLIENT_CONNECTED) {
		if (ReadBytes((char*)&header, 0, sizeof(header))) {
			checkSumHeader = header.checksum;
			if (header.type == (char)1) {
				if (ReadBytes((char*)&sP, &checksumCalced, sizeof(StatusPacket))) {
					if (checkSumHeader == checksumCalced) {
						if (sP.ID > statusPacket.ID) {
							memcpy(&statusPacket, &sP, sizeof(StatusPacket));
						}
					}
				}
			}
			else if (header.type == (char)2) {
				if (ReadBytes((char*)&vP, &checksumCalced, sizeof(VesselPacket))) {
					if (checkSumHeader == checksumCalced) {
						if (vP.ID > vesselPacket.ID) {
							memcpy(&vesselPacket, &vP, sizeof(VesselPacket));
						}
					}
				}
			}
		}
	}

	if (Running) {
		Shutdown();
	}
}

Client::Client()
{
	memset((char*)&controlPacket, 0, sizeof(controlPacket));
	memset((char*)&vesselPacket, 0, sizeof(vesselPacket));
	memset((char*)&statusPacket, 0, sizeof(statusPacket));
	memcpy(controlPacket.header.header, Header_Array, 8);
	controlPacket.SASTol = 0.05f;
	controlPacket.header.type = 1;
	controlPacket.ID = 0;
}

void Client::Connect(std::string IP, std::string PORT) {
	recLoop = std::thread(&Client::TCPClientRun, this, IP, PORT);
	recLoop.detach();
}

bool Client::Connected() {
	return state == TCPCLIENT_CONNECTED;
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

//helper methods

void Client::SetControlerMode(int controler, int mode) {
	switch (controler) {
	case CONTROLLER_ROT:
		controlPacket.ControlerMode = controlPacket.ControlerMode & 0b11111100 | mode << (2 * 0);
		break;
	case CONTROLLER_TRANS:
		controlPacket.ControlerMode = controlPacket.ControlerMode & 0b11110011 | mode << (2 * 1);
		break;
	case CONTROLLER_THROTTLE:
		controlPacket.ControlerMode = controlPacket.ControlerMode & 0b11001111 | mode << (2 * 2);
		break;
	case CONTROLLER_WHEEL:
		controlPacket.ControlerMode = controlPacket.ControlerMode & 0b00111111 | mode << (2 * 3);
		break;
	}
}

//Helper methods
void Client::ReSetSASHoldVector() {
	controlPacket.targetHeading = controlPacket.targetPitch = controlPacket.targetRoll = NAN;
}
void Client::SetSASHoldVector(float pitch, float heading, float roll) {
	controlPacket.targetHeading = heading;
	controlPacket.targetPitch = pitch;
	controlPacket.targetRoll = roll;
}

void Client::InputRot(float pitch, float yaw, float roll) {
	controlPacket.Pitch = (int16_t)pitch;
	controlPacket.Roll = (int16_t)roll;
	controlPacket.Yaw = (int16_t)yaw;
}

void Client::InputTran(float tx, float ty, float tz) {
	controlPacket.TX = (int16_t)tx;
	controlPacket.TY = (int16_t)ty;
	controlPacket.TZ = (int16_t)tz;
}

void Client::InputThrottle(float throttle) {
	controlPacket.Throttle = (int16_t)throttle;
}

bool Client::GetActionGroup(int group) {
	return (vesselPacket.ActionGroups & group) != 0;
}

void Client::SetActionGroup(int group, bool s) {
	if (s) {
		controlPacket.ActionGroups |= group;
	}
	else {
		controlPacket.ActionGroups &= ~((uint8_t)group);
	}
}


bool Client::GetMainControl(int control) {
	return (vesselPacket.MainControls & control) != 0;
}

void Client::SetMainControl(int control, bool s) {
	if (s) {
		controlPacket.MainControls |= control;
	}
	else {
		controlPacket.MainControls &= ~((uint8_t)control);
	}
}
