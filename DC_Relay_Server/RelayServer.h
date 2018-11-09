#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <mysql.h>

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
#define DC_ERRORLEVEL 0

#define MYSQL_ADDR "127.0.0.1"
#define MYSQL_PORT 3306
#define MYSQL_USER "root"
#define MYSQL_PASS "dogecloud"
#define MYSQL_DBASE "DogeCloud"

#endif

typedef struct {
	char srvAddr[255];
	unsigned long srvPort;
	char user[255];
	char pass[255];
	char dbase[255];
} MYSQL_SERVER;

typedef struct {
	SOCKET *hSocket;
	char clientIP[16];
} DC_CLIENT_INFO;

//extern var declare
extern HANDLE hMutex;
extern SOCKET hClientSocks[MAX_CON];
extern int clientCount;

//defining prototypes
unsigned int WINAPI clientHandler(void* arg);
void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode);

//Session Proccesors
void procLoginStart(SOCKET hClientSock);
void procLoginAccountData(SOCKET hClientSock);
void procLogout(SOCKET hClientSock);

//data Processors
void procFileUpDemo(SOCKET hClientSock);
void procFileDownDemo(SOCKET hClientSock);

//Mariadb Connector
void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo);