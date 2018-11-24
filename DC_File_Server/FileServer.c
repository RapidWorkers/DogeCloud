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
	@file FileServer.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief ���ϼ��� ���� ����
*/

#include "FileServer.h"

/**
	@var HANDLE hMutex
	�������� ��ȣ�� ���ؽ�
*/
HANDLE hMutex;
/**
@var SOCKET *hClientSocks
���� ����� ����ü �迭
*/
SOCKET *hClientSocks;
/**
@var DC_FILE_SESSION *sessionList
���� ����� ����ü �迭
*/
DC_FILE_SESSION *sessionList;
/**
@var DC_AUTHWAIT_LIST *authWaitList
���� ��⿭ ���� ����ü �迭
*/
DC_AUTHWAIT_LIST *authWaitList;
/**
@var int maxConnection
�ִ� ���Ӽ�
*/
int maxConnection;
/**
	@var int clientCount
	���� ī��Ʈ
*/
int clientCount;
/**
	@var int authWaitCount
	���� ��� ī��Ʈ
*/
int authWaitCount;
/**
	@var MYSQL_SERVER serverInfo
	MySQL ���� �ּ� ��� ����ü
*/
MYSQL_SERVER serverInfo;
/**
	@var MYSQL sqlHandle
	MySQL �ڵ�
*/
MYSQL sqlHandle;

/**
	@var int errorLevel
	����� ǥ�ÿ� ��������
*/
int errorLevel;

/**
	@fn int main()
	@brief ���ϼ��� ������
	@author �۸۾ƾ߿��غ�
*/
int main()
{
	/** @brief wsa ���̺귯���� ����ü */
	WSADATA wsaData;
	/** @brief ������ ����, Ŭ���̾�Ʈ ���� ó���� ���� */
	SOCKET hServSock, hClientSock;
	/** @brief ���� �ּ�, Ŭ���̾�Ʈ �ּ� ��� ����ü */
	SOCKADDR_IN servAddr, clientAddr;

	/** @brief ��Ƽ������ �ڵ� */
	HANDLE hThread = NULL;

	//unsigned int threadID;

	/** @brief ������ �ݷ��Ϳ� ��Ƽ������ �ڵ� */
	HANDLE hGCThread = NULL;

	if (!CreateDirectory("./storage", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {//�̹� ������ ������ ���� ��츦 ������ ��� ������ ���α׷� ����
			printDebugMsg(DC_ERROR, errorLevel, "Create Directory Fail");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}
	};

	//���ؽ� ����
	hMutex = CreateMutex(NULL, FALSE, NULL);
	//DWORD dwErrorCode = 0;

	//�������� �о���� ����
	setErrorLevel();

	maxConnection = readMaxConn();
	sessionList = (DC_FILE_SESSION*)calloc(maxConnection, sizeof(DC_FILE_SESSION));
	hClientSocks = (SOCKET*)calloc(maxConnection, sizeof(SOCKET));
	authWaitList = (DC_AUTHWAIT_LIST*)calloc(maxConnection, sizeof(DC_AUTHWAIT_LIST));

	if (sessionList == NULL || hClientSocks == NULL || authWaitList == NULL) {
		printDebugMsg(DC_ERROR, errorLevel, "Memory Allocation Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	printProgramInfo();

	//�����ͺ��̽� ���� �а� ����
	readMySQLConfig(&serverInfo);
	sqlInit(&sqlHandle, serverInfo);

	//���� �ʱ�ȭ ����
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {//WSA Startup
		printDebugMsg(DC_ERROR, errorLevel, "WSAStartup Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);//���� ���� �ʱ�ȭ
	if (hServSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, errorLevel, "Invalid Server Socket");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	//���� �ּ� ����
	readBindInfo(&servAddr);

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) {//���ε�
		printDebugMsg(DC_ERROR, errorLevel, "Bind Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	if (listen(hServSock, 5) == SOCKET_ERROR) {//���� ������ ����
		printDebugMsg(DC_ERROR, errorLevel, "Listen Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	printDebugMsg(DC_INFO, errorLevel, "Server Started");
	char serverIP[16] = { 0, };
	inet_ntop(AF_INET, &servAddr.sin_addr, serverIP, 16);
	printDebugMsg(DC_INFO, errorLevel, "Server Listening at %s:%d", serverIP, ntohs(servAddr.sin_port));//������ ���� ǥ��

	//������ �ݷ��� ����
	printDebugMsg(DC_INFO, errorLevel, "Starting Garbage Collector");
	hGCThread = (HANDLE)_beginthreadex(NULL, 0, waitingListGC, NULL, 0, NULL);
	if (hGCThread == NULL) {
		printDebugMsg(DC_ERROR, errorLevel, "GC Start Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}
	printDebugMsg(DC_INFO, errorLevel, "Done Starting Garbage Collector");

	//Ŭ���̾�Ʈ ���� ���
	while (1) {
		int szClntAddr = sizeof(clientAddr);
		hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &szClntAddr);

		if (hClientSock == INVALID_SOCKET) {//Ŭ���̾�Ʈ ���� ���н�
			printDebugMsg(DC_ERROR, errorLevel, "Client Accept Fail");
			break;
		}

		if (clientCount >= maxConnection) {//���� ���� ���޽�
			printDebugMsg(DC_WARN, errorLevel, "MAX CONNECTION REACHED, CLOSE CONNECTION!!");
			closesocket(hClientSock);
			continue;
		}

		WaitForSingleObject(hMutex, INFINITE);
		hClientSocks[clientCount++] = hClientSock;
		/** @brief Ŭ���̾�Ʈ ���� ���� ����ü */
		DC_SOCK_INFO clientInfo;
		clientInfo.hSocket = &hClientSock;
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientInfo.clientIP, 16);//Ŭ���̾�Ʈ ip ���ڿ��� ��ȯ
		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, clientHandler, (void*)&clientInfo, 0, NULL);//������ �����Ͽ� �ѱ�
		printDebugMsg(DC_INFO, errorLevel, "Client Connected: %s", clientInfo.clientIP);
		printDebugMsg(DC_INFO, errorLevel, "Connection Limit: %d / %d", clientCount, maxConnection);
	}

	closesocket(hServSock);//���� ����� ���� ����
	WSACleanup();//���̺귯�� ����

	free(sessionList);
	free(hClientSocks);
	free(authWaitList);
	printDebugMsg(1, errorLevel, "Server Terminated");
	system("pause");
	return 0;
}