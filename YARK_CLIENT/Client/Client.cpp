#define _CRT_SECURE_NO_WARNINGS

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

uint16_t checksum(uint8_t *buffer, int length) {
	uint16_t acc = 0;
	for (int i = 0; i < length; i++) {
		acc += buffer[i];
	}
	return (uint16_t)acc;
}

void Client::SendControls() { //use async ?
	Control.header.checksum = checksum((uint8_t*)&Control.ID, sizeof(Control) - sizeof(Header));
	Send((char*)&Control, sizeof(Control));
	Control.ID++;
}

void Client::SendManChange(uint8_t mode, uint8_t ID, float UT, glm::vec3 vector) {
	ManChangePacket.mode = mode;
	ManChangePacket.manID = ID;
	ManChangePacket.UT = UT;
	ManChangePacket.X = vector.x;
	ManChangePacket.Y = vector.y;
	ManChangePacket.Z = vector.z;
	Send((char*)&ManChangePacket, sizeof(ManChangePacket));
}

void Client::Send(char *buff, int length) {
	int	iResult = send(ConnectSocket, buff, length, 0);
	if (iResult == SOCKET_ERROR) {
		int errorN = WSAGetLastError();
		if (errorN != WSAEWOULDBLOCK) {
			sprintf(error, "error sending: %d", errorN);
			WSACleanup();
		}
	}
}

bool Client::ReadBytes(char *buffer, uint16_t *checkSum, int bytesToRead) {
	int bytesRead = 0;
	while (bytesRead < bytesToRead) {
		int result = recv(ConnectSocket, buffer + bytesRead, bytesToRead - bytesRead, 0);
		if (result > 0) {
			bytesRead += result;
		}
		else if (result == 0) {
			sprintf(error, "Connection Closed");
			state = TCP_FAILED;
			return false;
		}
		else {
			int errorN = WSAGetLastError();
			if (errorN != WSAEWOULDBLOCK) {
				state = TCP_FAILED;
				sprintf(error, "Recv Failed: %d", errorN);
				return false;
			}
		}
	}
	if (checkSum) {
		*checkSum = checksum((uint8_t*)buffer, bytesToRead);
	}
	return true;
}

void Client::errBadPacket() {
	sprintf(error, "Malformed Packet");
	state = TCP_FAILED;
	Running = false;
}

void Client::Run(std::string IP, std::string PORT) {
#pragma region winsock stuff
	state = TCP_CONNECTING;
	WSADATA wsaData;
	struct addrinfo *result = NULL,
		*ptr = NULL,
		hints;

	int iResult;
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		sprintf(error, "WSAStartup failed");

		state = TCP_FAILED;
		return;
	}
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	iResult = getaddrinfo(IP.c_str(), PORT.c_str(), &hints, &result);
	if (iResult != 0) {
		sprintf(error, "getaddrinfo failed: %d", iResult);
		state = TCP_FAILED;
		WSACleanup();
		return;
	}

	for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
		ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
		if (ConnectSocket == INVALID_SOCKET) {
			state = TCP_FAILED;
			sprintf(error, "INVALID_SOCKET: %ld\n", WSAGetLastError());
			WSACleanup();
			return;
		}

		iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			sprintf(error, "SOCKET_ERROR: %ld\n", WSAGetLastError());
			closesocket(ConnectSocket);
			ConnectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(result);

	if (ConnectSocket == INVALID_SOCKET) {
		sprintf(error, "Unable to connect to server!");
		state = TCP_FAILED;
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

	state = TCP_CONNECTED;

#pragma endregion
	Running = true;
	StatusPacket sP;
	VesselPacket vP;
	char data[1024];

	Header header;

	uint16_t checksumCalced;

	while (Running && state == TCP_CONNECTED) {
		if (ReadBytes((char*)&header, 0, sizeof(header))) {
			if (!memcmp(header.header, Header_Array, sizeof(Header_Array))) {
				if (header.type == (char)1) {
					if (header.length == sizeof(StatusPacket)) {
						if (ReadBytes((char*)&sP, &checksumCalced, sizeof(StatusPacket))) {
							if (header.checksum == checksumCalced) {
								if (sP.ID > Status.ID) {
									memcpy(&Status, &sP, sizeof(StatusPacket));
									if (Status.YarkVersion != 0x03) {
										printf("Bad version: %d\n", Status.YarkVersion);
									}
								}
							}
						}
					}
					else {
						errBadPacket();
					}
				}
				else if (header.type == (char)2) {
					if (header.length == sizeof(VesselPacket)) {
						if (ReadBytes((char*)&vP, &checksumCalced, sizeof(VesselPacket))) {
							if (header.checksum == checksumCalced) {
								if (vP.ID >= Vessel.ID) {
									memcpy(&Vessel, &vP, sizeof(VesselPacket));
								}
							}
						}
					}
					else {
						errBadPacket();
					}
				}
				else if (header.type == (char)3) {
					if (ReadBytes(data, &checksumCalced, header.length)) {
						int offset = 0;

						int numOrbits = data[offset]; offset++; //CurrentOrbitPatches
						OrbitPlan.CurrentOrbitPatches.resize(numOrbits);
						for (int i = 0; i < numOrbits; i++) {
							memcpy(&OrbitPlan.CurrentOrbitPatches[i], data + offset, sizeof(OrbitData));
							offset += sizeof(OrbitData);
						}

						OrbitPlan.ManPatchNum = data[offset]; offset++;

						int numOrbitsPlanned = data[offset]; offset++; //PlannedOrbitPatches
						OrbitPlan.PlannedOrbitPatches.resize(numOrbitsPlanned);
						for (int i = 0; i < numOrbitsPlanned; i++) {
							memcpy(&OrbitPlan.PlannedOrbitPatches[i], data + offset, sizeof(OrbitData));
							offset += sizeof(OrbitData);
						}
						memcpy(&OrbitPlan.TargetOrbit, data + offset, sizeof(OrbitData)); //targetorbit
						offset += sizeof(OrbitData);

						//targetname
						OrbitPlan.TargetName = std::string(data + offset + 1);
						offset += data[offset] + 2;

						memcpy(&OrbitPlan.CAD, data + offset, sizeof(OrbitPlan.CAD));
						offset += sizeof(OrbitPlan.CAD);

						int numMans = data[offset]; offset++;
						OrbitPlan.Mans.resize(numMans);
						for (int i = 0; i < numMans; i++) {
							memcpy(&OrbitPlan.Mans[i], data + offset, sizeof(ManData));
							offset += sizeof(ManData);
						}
					}
				}
				else {
					printf("huh\n");
					ReadBytes(data, &checksumCalced, header.length);
				}
			}
			else {
				//errBadPacket();
			}
		}
		else {
			errBadPacket();
		}
	}

	if (Running) {
		Shutdown();
	}
}

Client::Client()
{
	memset(error, 0, sizeof(error));
	sprintf(error, "none");
	memset((char*)&Control, 0, sizeof(ControlPacket));
	memset((char*)&Vessel, 0, sizeof(VesselPacket));
	memset((char*)&Status, 0, sizeof(StatusPacket));
	memcpy(Control.header.header, Header_Array, 8);
	Control.SASTol = 0.05f;
	Control.header.type = 1;
	Control.ID = 0;
	memcpy(ManChangePacket.header.header, Header_Array, 8);
	ManChangePacket.header.type = 2;
}

void Client::Connect(std::string IP, std::string PORT) {
	recLoop = std::thread(&Client::Run, this, IP, PORT);
	recLoop.detach();
}

bool Client::Connected() {
	return state == TCP_CONNECTED;
}

int Client::GetState() {
	return state;
}

void Client::WaitForConnection() {
	while (GetState() == TCP_CONNECTING) {} //wait for connection	
}

void Client::Shutdown() {
	state = TCP_FAILED;
	Running = false;
	if (shutdown(ConnectSocket, SD_SEND) == SOCKET_ERROR) {
		sprintf(error, "shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
	}
}


