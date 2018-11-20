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

#define sendData(hClientSock, packetBuffer, packetSize, flag) \
if (!sendRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");\
	system("pause");\
	exit(1);\
}

#define recvData(hClientSock, packetBuffer, packetSize, flag) \
if (!recvRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");\
	system("pause");\
	exit(1);\
}

//session related global var
extern int loginFlag;
extern char currentUsername[100];
extern char sessionKey[32];

//AuthHelper
void userLogin(SOCKET hSocket);
void userRegister(SOCKET hSocket);
void userLogout(SOCKET hSocket);

//userDataHelper
void manageFile(SOCKET hSocket);
void manageContacts(SOCKET hSocket);
void manageMemo(SOCKET hSocket);

//personalDBHelper
void downloadPersonalDBFile(SOCKET hSocket);
void uploadPersonalDBFile(SOCKET hSocket, char* originalHash);
void addContacts();
void modifyContacts();
void addMemo();
void modifyMemo();

//configReader
void checkDogeConfig();
void readRelayServerPath(SOCKADDR_IN *RelayServAddr);

//fileServerConnector
void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey);
void doFileManage(SOCKET hFileSrvSock);
void showFileList(SOCKET hFileSrvSock, int *errorFlag);
void moveDir(SOCKET hFileSrvSock, int *errorFlag);
void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag);
void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag);