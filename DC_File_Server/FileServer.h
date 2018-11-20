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
#define BIND_PORT 15332
#define MAX_CON 100
#define DC_ERRORLEVEL 0
#endif

typedef struct {
	char srvAddr[255];
	unsigned long srvPort;
	char user[255];
	char pass[255];
	char dbase[255];
} MYSQL_SERVER;

typedef struct {
	char authKey[32];
	unsigned int authWaitTime;
	unsigned long userUID;
} DC_AUTHWAIT_LIST;

typedef struct {
	char authKey[32];
	unsigned long userUID;
	char directory[255];
} DC_FILE_SESSION;

//defining macro function for handling error on send and receive packet
#define sendData(hClientSock, packetBuffer, packetSize, flag) \
if (!sendRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());\
	return;\
}

#define recvData(hClientSock, packetBuffer, packetSize, flag) \
if (!recvRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());\
	return;\
}

//extern var declare
extern HANDLE hMutex;
extern SOCKET hClientSocks[MAX_CON];
extern DC_FILE_SESSION sessionList[MAX_CON];
extern DC_AUTHWAIT_LIST authWaitList[MAX_CON];
extern int clientCount;
extern int authWaitCount;

extern MYSQL_SERVER serverInfo;
extern MYSQL sqlHandle;

//function prototypes
unsigned int WINAPI clientHandler(void* clientInfo);
void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode);

//Mariadb Connector
void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo);
void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind);

//Configuration Reader
void checkFileConfig();
void readMySQLConfig(MYSQL_SERVER *serverInfo);

//fileServerRegister Manager
void procRegisterFileServer(SOCKET hClientSock);

//user AuthManager
void procAddUserAuthWaitList(SOCKET hClientSock);
void procFileLogin(SOCKET hClientSock);

//garbage collector
void waitingListGC();

//userFileManager
void procListFile(SOCKET hClientSock);