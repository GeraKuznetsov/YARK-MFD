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

void Client::SendControls() { //use async
	//return;
	ControlPacket.header.checksum = checksum((uint8_t*)&ControlPacket.ID, sizeof(ControlPacket) - sizeof(Header));
	int	iResult = send(ConnectSocket, (char*)&ControlPacket, sizeof(ControlPacket), 0);
	if (iResult == SOCKET_ERROR) {
		int errorN = WSAGetLastError();
		if (errorN != WSAEWOULDBLOCK) {
			printf("send failed with error: %d\n", errorN);
			closesocket(ConnectSocket);
			WSACleanup();
		}
	}
	ControlPacket.ID++;
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


#if false
	do {
		if ((iResult = recv(ConnectSocket, buffer + bp, RECV_BUFF_SIZE - bp, 0)) > 0) {
			bp += iResult;
			int p = 0;

			//int needBytes = lookingForHeader? (8+1):

			while (p + FIXED_PACKETSIZE <= bp) {
				if (!memcmp(buffer + p, Header_Array, sizeof(Header_Array))) {
					p += sizeof(Header_Array);
					uint8_t packetType = *(buffer + p);
					p++;
					if (packetType == (char)1) {
						memcpy((char*)&sP, buffer + p, sizeof(StatusPacket));
						if (sP.ID > statusPacket.ID) {
							memcpy(&statusPacket, &sP, sizeof(StatusPacket));
						}
						printf("SP\n");
						p += sizeof(StatusPacket);
					}
					else if (packetType == (char)2) {
						memcpy((char*)&vP, buffer + p, sizeof(VesselPacket));
						if (vP.ID > vesselPacket.ID) {
							memcpy(&vesselPacket, &vP, sizeof(VesselPacket));
						}
						printf("VP\n");
						p += sizeof(VesselPacket);
					}
					else {
						printf("bad packettype\n");
					}
				}
				else {
					p++;
				}
			}
			memcpy(buffer, buffer + p, bp - p);
			bp = bp - p;
		}
		else if (iResult == 0) {
			error = "recv failed";
			state = TCPCLIENT_FAILED;
			printf("Connection closed\n");
			error = true;
		}
		else {
			int errorN = WSAGetLastError();
			if (errorN != WSAEWOULDBLOCK) {
				error = "con closed";
				state = TCPCLIENT_FAILED;
				printf("recv failed with error: %d\n", errorN);
				error = true;
			}
}
	} while (!error && Running);
#endif

	if (Running) {
		Shutdown();
	}
}

Client::Client()
{
	memset((char*)&ControlPacket, 0, sizeof(ControlPacket));
	memset((char*)&vesselPacket, 0, sizeof(vesselPacket));
	memset((char*)&statusPacket, 0, sizeof(statusPacket));
	memcpy(ControlPacket.header.header, Header_Array, 8);
	ControlPacket.header.type = 1;
	ControlPacket.ID = 0;
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
bool Client::GetMainControls(int control) {
	return (vesselPacket.MainControls & control) != 0;
}

void Client::SetMainControls(int control, bool s) {
	if (s) {
		ControlPacket.MainControls |= control;
	}
	else {
		ControlPacket.MainControls &= ~((uint8_t)control);
	}
}
