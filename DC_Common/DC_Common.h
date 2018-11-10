#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <stdarg.h>
#pragma comment(lib, "ws2_32.lib")

//load packet structures
#include "DataManagePacket.h"
#include "SessionPacket.h"
#include "UserCommPacket.h"
#include "FileServerCommPacket.h"

typedef struct {
	SOCKET *hSocket;
	char clientIP[16];
} DC_SOCK_INFO;


#ifdef DC_BUILD_DLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
//NOTE: Packet operation Code is defined at *Packet.h

//Program info
#define MAJOR_VERSION 0
#define MINOR_VERSION 1
#define VER_STATUS "In-dev"

#define DC_ERROR 3
#define DC_WARN 2
#define DC_INFO 1
#define DC_DEBUG 0

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif
	//prototype for dll exported function, which used in other program to load function

	//print something Function
	DLL void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...);
	DLL void printProgramInfo();

	//Cryptography Function
	DLL void SHA256_Text(const char* text, char* buf);
	DLL void GenerateSessionKey(char sessionKey[32]);
	DLL void GenerateCSPRNG(unsigned char *buffer, int numSize);

	//Network Related Function
	DLL bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags);
	DLL bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags);

	//File Related Function
	DLL void getFileHash(FILE *file, char* result);
	DLL void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV);
	DLL void decryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV);

#ifdef __cplusplus
}
#endif