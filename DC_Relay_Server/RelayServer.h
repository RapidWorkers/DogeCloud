#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include "../DC_Common/DC_Common.h"

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DC_Common.lib")

#define DEVELOPEMENT_MODE

//FOR TESTING ONLY
#ifdef DEVELOPEMENT_MODE
#define BUF_SIZE 2048
#define QUEUE_SIZE 10
#define BIND_ADDR "127.0.0.1"
#define BIND_PORT 15754
#define MAX_CON 100
#define ERLEVEL 0
#endif

//Mutex init
HANDLE hMutex;

//create Client Sock array
SOCKET hClientSocks[MAX_CON];
int clientCount;

//defining prototypes
unsigned int WINAPI clientHandler(void* arg);