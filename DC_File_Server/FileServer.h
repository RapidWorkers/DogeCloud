/*
Copyright (C) 2018 S.H.Kim (soohyunkim@kw.ac.kr)
Copyright (C) 2018 K.J Choi (chlrhkdwls99@naver.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/**
	@file FileServer.h
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief FileServer ���� ��� ����
*/

#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <mysql.h>
#include <time.h>

#include "../DC_Common/DC_Common.h"

//���� ���̺귯�� �ε�
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

//����ü ���� ����
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

/**
	@fn sendData(hClientSock, packetBuffer, packetSize, flag)
	@brief ��Ŷ ���� ó���� ��ũ�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����
	@param packetBuffer ��Ŷ ����
	@param packetSize ��Ŷ ������
	@param flag ���� �÷���
*/
#define sendData(hClientSock, packetBuffer, packetSize, flag) \
if (!sendRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());\
	return;\
}

/**
	@fn recvData(hClientSock, packetBuffer, packetSize, flag)
	@brief ��Ŷ ���� ó���� ��ũ�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����
	@param packetBuffer ��Ŷ ����
	@param packetSize ��Ŷ ������
	@param flag ���� �÷���
*/
#define recvData(hClientSock, packetBuffer, packetSize, flag) \
if (!recvRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());\
	return;\
}

//�������� ����
/**
	@var HANDLE hMutex
	�������� ��ȣ�� ���ؽ�
*/
extern HANDLE hMutex;
/**
	@var SOCKET hClientSocks[MAX_CON]
	���� ����� ����ü �迭
*/
extern SOCKET hClientSocks[MAX_CON];
/**
	@var DC_FILE_SESSION sessionList[MAX_CON]
	���� ����� ����ü �迭
*/
extern DC_FILE_SESSION sessionList[MAX_CON];
/**
	@var DC_AUTHWAIT_LIST authWaitList[MAX_CON]
	���� ��⿭ ���� ����ü �迭
*/
extern DC_AUTHWAIT_LIST authWaitList[MAX_CON];
/**
	@var int clientCount
	���� ī��Ʈ
*/
extern int clientCount;
/**
	@var int authWaitCount
	���� ��� ī��Ʈ
*/
extern int authWaitCount;
/**
	@var MYSQL_SERVER serverInfo
	MySQL ���� �ּ� ��� ����ü
*/
extern MYSQL_SERVER serverInfo;
/**
	@var MYSQL sqlHandle
	MySQL �ڵ�
*/
extern MYSQL sqlHandle;

//socket Handler
/**
	@fn unsigned int WINAPI clientHandler(void* clientInfo)
	@brief Ŭ���̾�Ʈ �ڵ鷯
	@author �۸۾ƾ߿��غ�
	@param clientInfo (DC_SOCK_INFO)�� void*�� ĳ������ ������
*/
unsigned int WINAPI clientHandler(void* clientInfo);

/**
	@fn void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode)
	@brief ��Ŷ �ڵ鷯
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ�� ����� ����
	@param *clientIP Ŭ���̾�Ʈ IP
	@param opCode ��Ŷ opCode
*/
void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode);

//Mariadb Connector
/**
	@fn void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo)
	@brief SQL ���� ����
	@author �۸۾ƾ߿��غ�
	@param *sqlHandle MySQL Handle ������
	@param serverInfo MySQL ���� ���� ����ü
*/
void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo);

/**
	@fn void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind)
	@brief SQL Prepared Statement �غ� �� ���� ����
	@author �۸۾ƾ߿��غ�
	@param *sqlHandle MySQL Handle ������
	@param *stmt MySQL stmt ������
	@param *query ������ ������
	@param *query_bind ������ ���ε��� ���� ������
	@param *result_bind ������� ���ε��� ���� ������
*/
void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind);

//Configuration Reader
/**
	@fn void checkFileConfig()
	@brief ���ϼ��� �������� ���� �˻�
	@author �۸۾ƾ߿��غ�
*/
void checkFileConfig();

/**
	@fn void readMySQLConfig(MYSQL_SERVER *serverInfo)
	@brief MySQL ���� ���� �о��
	@author �۸۾ƾ߿��غ�
	@param *serverInfo MySQL ���� ���� ����ü ������
*/
void readMySQLConfig(MYSQL_SERVER *serverInfo);

//fileServerRegister Manager
/**
	@fn void procRegisterFileServer(SOCKET hClientSock)
	@brief �߰輭���κ����� ���ϼ��� ��Ͽ�û�� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock �߰輭�� ����� ����
*/
void procRegisterFileServer(SOCKET hClientSock);

//user AuthManager
/**
	@fn void procAddUserAuthWaitList(SOCKET hClientSock)
	@brief �߰輭���κ����� ���� ������û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock �߰輭�� ����� ����
*/
void procAddUserAuthWaitList(SOCKET hClientSock);

/**
	@fn void procFileLogin(SOCKET hClientSock)
	@brief Ŭ���̾�Ʈ�� ���ϼ��� �α��� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procFileLogin(SOCKET hClientSock);

//garbage collector
/**
	@fn unsigned int WINAPI waitingListGC()
	@brief ��⿭ ������ �ݷ���, �����ð��� �ʰ��� ��⿭�� ������
	@author �۸۾ƾ߿��غ�
*/
unsigned int WINAPI waitingListGC();

//userFileManager
/**
	@fn void procListFile(SOCKET hClientSock)
	@brief ���� ����Ʈ ����
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procListFile(SOCKET hClientSock);