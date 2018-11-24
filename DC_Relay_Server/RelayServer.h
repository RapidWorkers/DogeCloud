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
	@file RelayServer.h
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief RelayServer ���� ��� ����
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

#include "../DC_Common/DC_Common.h"

//dll �ε�
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DC_Common.lib")

typedef struct {
	char srvAddr[255];
	unsigned long srvPort;
	char user[255];
	char pass[255];
	char dbase[255];
} MYSQL_SERVER;

typedef struct {
	unsigned long userUID;
	char currentStatus;
	char sessionKey[32];
} DC_SESSION;

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
	printDebugMsg(DC_ERROR, errorLevel, "Connection Error: %d", WSAGetLastError());\
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
	printDebugMsg(DC_ERROR, errorLevel, "Connection Error: %d", WSAGetLastError());\
	return;\
}


//�������� ����
/**
	@var HANDLE hMutex
	�������� ��ȣ�� ���ؽ�
*/
extern HANDLE hMutex;
/**
@var SOCKET *hClientSocks
���� ����� ����ü �迭 ������(�����Ҵ����� ������)
*/
extern SOCKET *hClientSocks;
/**
@var DC_SESSION *sessionList
���� ����� ����ü �迭 ������(�����Ҵ����� ������)
*/
extern DC_SESSION *sessionList;

/**
@var int maxConnection
�ִ� ���Ӽ�
*/
extern int maxConnection;

/**
	@var int clientCount
	���� ī��Ʈ
*/
extern int clientCount;

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

/**
	@var SOCKADDR_IN fileSrvAddr
	���ϼ��� �ּ� ����� ����ü
*/
extern SOCKADDR_IN fileSrvAddr;
/**
	@var SOCKET hFileSrvSock
	���ϼ��� ����
*/
extern SOCKET hFileSrvSock;
/**
	@var int fileServerConnectFlag
	���ϼ��� ���� �÷���
*/
extern int fileServerConnectFlag;

/**
	@var int errorLevel
	����� ǥ�ÿ� ��������
*/
extern int errorLevel;

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

//Session Proccesors

/**
	@fn void procLoginStart(SOCKET hClientSock)
	@brief �α��� ó�� ����
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procLoginStart(SOCKET hClientSock);

/**
	@fn void doLogin(SOCKET hClientSock)
	@brief ���� �α��� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void doLogin(SOCKET hClientSock);

/**
	@fn void procLogout(SOCKET hClientSock)
	@brief �α׾ƿ� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procLogout(SOCKET hClientSock);

/**
	@fn void procRegisterStart(SOCKET hClientSock)
	@brief ȸ������ ó�� ����
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procRegisterStart(SOCKET hClientSock);

/**
	@fn void doRegister(SOCKET hClientSock)
	@brief ���� ȸ������ ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void doRegister(SOCKET hClientSock);

//Configuration Reader
/**
	@fn void checkRelayConfig()
	@brief �߰輭�� �������� ���� �˻�
	@author �۸۾ƾ߿��غ�
*/
void checkRelayConfig();

/**
	@fn void setErrorLevel()
	@brief �������� ����
	@author �۸۾ƾ߿��غ�
*/
void setErrorLevel();

/**
@fn int readMaxConn()
@brief �ִ� ���Ӽ� ��ȯ
@author �۸۾ƾ߿��غ�
@return �ִ� ���Ӽ�
*/
int readMaxConn();

/**
@fn void readBindInfo(SOCKADDR_IN *servAddr)
@brief ���� IP, ��Ʈ ���� �о��
@author �۸۾ƾ߿��غ�
@param *servAddr ���� �ּ� ���� ���� ����ü ������
*/
void readBindInfo(SOCKADDR_IN *servAddr);

/**
	@fn void readMySQLConfig(MYSQL_SERVER *serverInfo)
	@brief MySQL ���� ���� �о��
	@author �۸۾ƾ߿��غ�
	@param *serverInfo MySQL ���� ���� ����ü ������
*/
void readMySQLConfig(MYSQL_SERVER *serverInfo);

/**
	@fn void readFileServerPath(SOCKADDR_IN *FileServAddr)
	@brief DogeCloud ���� ���� ��� �о����
	@author �۸۾ƾ߿��غ�
	@param *FileServAddr �߰� ���� ����� �ּ� ����ü �ּ�
*/
void readFileServerPath(SOCKADDR_IN *FileServAddr);

//fileServer Register
/**
	@fn void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *FileServAddr)
	@brief ���ϼ��� ����
	@author �۸۾ƾ߿��غ�
	@param *hFileSrvSock ���ϼ��� ����� ����
	@param *FileServAddr �߰� ���� ����� �ּ� ����ü �ּ�
*/
void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *FileServAddr);

//user to FileServer bridge
/**
	@fn void procFileServerConnReq(SOCKET hClientSock)
	@brief ������� ���ϼ��� �����û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����� ����� ����
*/
void procFileServerConnReq(SOCKET hClientSock);

/**
	@fn void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag)
	@brief ����ڸ� ���ϼ����� ������Ŵ
	@author �۸۾ƾ߿��غ�
	@param *authKey ����Ű ���ڿ�
	@param userUID ���� UID
	@param resultFlag ��� �÷���
*/
void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag);

//personalDBHandler
/**
	@fn void procDownloadPersonalDBFile(SOCKET hClientSock)
	@brief ������� ���� DB �ٿ�ε� ��û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����� ����� ����
*/
void procDownloadPersonalDBFile(SOCKET hClientSock);
/**
	@fn void procUploadPersonalDBFile(SOCKET hClientSock)
	@brief ������� ���� DB ���ε� ��û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����� ����� ����
*/
void procUploadPersonalDBFile(SOCKET hClientSock);