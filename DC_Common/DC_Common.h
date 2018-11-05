#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#pragma comment(lib, "ws2_32.lib")

//load packet structures
#include "DataManagePacket.h"
#include "SessionPacket.h"
#include "UserCommPacket.h"
#include "FileServerCommPacket.h"

#define DLL _declspec(dllexport)
#define DLLIMP _declspec(dllimport)

//NOTE: Packet operation Code is defined at *Packet.h

//Program info
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define VER_STATUS "In-dev"

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif
	//prototype for dll exported function, which used in other program to load function
	DLLIMP void SHA256_Text(const char* text, char* buf);
	DLLIMP void testLEA();
	DLLIMP void printDebugMsg(int targetErrorLevel, int currentErrorLevel, char* buffer);
	DLLIMP void printProgramInfo();
	DLLIMP unsigned int GenerateCSPRNG();
	DLLIMP void GenerateSessionKey(char sessionKey[32]);

	//Network Related Function
	DLLIMP bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags);
	DLLIMP bool recvRaw(SOCKET socket, char* buffer, int sendByte, int flags);

#ifdef __cplusplus
}
#endif