#pragma once
#include "DataManagePacket.h"
#include "SessionPacket.h"
#include "UserCommPacket.h"
#include "FileServerCommPacket.h"

#define DLLIMP _declspec(dllimport)

#ifdef __cplusplus //check if cpp compiler compile this code.
extern "C" {
#endif
	DLLIMP void myFunc();//prototype for dll exported function, which used in other program to load function
	DLLIMP void testLEA();
	DLLIMP void testPacketStructure();
	DLLIMP void testSHA256();
	DLLIMP void printDebugMsg(int targetErrorLevel, int currentErrorLevel, char* buffer);

#ifdef __cplusplus
}
#endif