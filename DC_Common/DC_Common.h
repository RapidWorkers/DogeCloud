#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//load packet structures
#include "DataManagePacket.h"
#include "SessionPacket.h"
#include "UserCommPacket.h"
#include "FileServerCommPacket.h"

#define DLLIMP _declspec(dllimport)

//NOTE: Packet operation Code is defined at *Packet.h

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif
	//prototype for dll exported function, which used in other program to load function
	DLLIMP void SHA256_Text(const char* text, char* buf);
	DLLIMP void testLEA();
	DLLIMP void testSHA256();
	DLLIMP void printDebugMsg(int targetErrorLevel, int currentErrorLevel, char* buffer);

#ifdef __cplusplus
}
#endif