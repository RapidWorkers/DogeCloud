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
#define BIND_PORT 15384
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

//for socket and session
extern SOCKET hClientSocks[MAX_CON];
extern char sessionKey[MAX_CON][32];
extern char currentStatus[MAX_CON];
extern int clientCount;

//for mysql
extern MYSQL_SERVER serverInfo;
extern MYSQL sqlHandle;

//fileServer Connected check
extern SOCKADDR_IN fileSrvAddr;
extern SOCKET hFileSrvSock;
extern int fileServerConnectFlag;

//socket Handler
unsigned int WINAPI clientHandler(void* clientInfo);
void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode);

//Session Proccesors
void procLoginStart(SOCKET hClientSock);
void doLogin(SOCKET hClientSock);
void procLogout(SOCKET hClientSock);
void procRegisterStart(SOCKET hClientSock);
void doRegister(SOCKET hClientSock);

//data Processors
void procFileUpDemo(SOCKET hClientSock);
void procFileDownDemo(SOCKET hClientSock);

//Mariadb Connector
void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo);
void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind);

//Configuration Reader
void checkRelayConfig();
void readMySQLConfig(MYSQL_SERVER *serverInfo);
void readFileServerPath(SOCKADDR_IN *FileServAddr);

//fileServer Register
void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *FileServAddr);

//user to FileServer bridge
void procFileServerConnReq(SOCKET hClientSock);
void authFSUser(char* authKey, int *resultFlag);