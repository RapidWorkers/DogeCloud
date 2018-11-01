#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "../DC_Common/DC_Common.h"

#pragma comment(lib, "DC_Common.lib")
#pragma comment(lib, "ws2_32.lib")

//session related global var
extern int loginFlag;
extern char sessionKey[32];

//AuthHelper
void login(SOCKET hSocket);
void logout(SOCKET hSocket);

//fileHelper
void fileUpDemo(SOCKET hSocket);
void fileDownDemo(SOCKET hSocket);