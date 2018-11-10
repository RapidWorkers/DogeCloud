#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "../DC_Common/DC_Common.h"

#pragma comment(lib, "DC_Common.lib")
#pragma comment(lib, "ws2_32.lib")

#define DC_ERRORLEVEL 0

#define clearStdinBuffer() while(getchar() != '\n')

//session related global var
extern int loginFlag;
extern char currentUsername[100];
extern char sessionKey[32];

//AuthHelper
void userLogin(SOCKET hSocket);
void userRegister(SOCKET hSocket);
void userLogout(SOCKET hSocket);

//fileHelper
void fileUpDemo(SOCKET hSocket);
void fileDownDemo(SOCKET hSocket);
void testLEAonFILE();
void testSHA();

//configReader
void checkRelayConfig();