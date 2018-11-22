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
	@file RelayServer.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief RelayServer ���� ����
*/

#include "RelayServer.h"

/**
	@var HANDLE hMutex
	�������� ��ȣ�� ���ؽ�
*/
HANDLE hMutex;

//create Client Sock array
/**
	@var SOCKET hClientSocks[MAX_CON]
	���� ����� ����ü �迭
*/
SOCKET hClientSocks[MAX_CON];
/**
	@var DC_SESSION sessionList[MAX_CON]
	���� ����� ����ü �迭
*/
DC_SESSION sessionList[MAX_CON];
/**
	@var int clientCount
	���� ī��Ʈ
*/
int clientCount;

/**
	@var SOCKET hFileSrvSock
	���ϼ��� ����
*/
SOCKET hFileSrvSock;
/**
	@var SOCKADDR_IN fileSrvAddr
	���ϼ��� �ּ� ����� ����ü
*/
SOCKADDR_IN fileSrvAddr;
/**
	@var int fileServerConnectFlag
	���ϼ��� ���� �÷���
*/
int fileServerConnectFlag;

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
	@brief RelayServer ������
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

	//���ؽ� ����
	hMutex = CreateMutex(NULL, FALSE, NULL);
	//DWORD dwErrorCode = 0;

	//�������� �о���� ����
	setErrorLevel();

	printProgramInfo();

	//�����ͺ��̽� ���� �а� ����
	readMySQLConfig(&serverInfo);
	sqlInit(&sqlHandle, serverInfo);

	//����ó, �޸� db ����� ���� ����
	if (!CreateDirectory("./infodb", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {//�̹� ������ ������ ���� ��츦 ������ ��� ������ ���α׷� ����
			printDebugMsg(DC_ERROR, errorLevel, "Create Directory Fail");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}
	};

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
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(BIND_PORT);

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) {//���ε�
		printDebugMsg(DC_ERROR, errorLevel, "Bind Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	//���ϼ��� ��� �о����
	readFileServerPath(&fileSrvAddr);
	for (int i = 0; i < 5; i++) {//���ϼ��� 5�� ���� �õ�
		printDebugMsg(DC_INFO, errorLevel, "Trying to Connect File Server %d / 5", i+1);
		initFSConnection(&hFileSrvSock, &fileSrvAddr);
		if (fileServerConnectFlag) break;
		Sleep(1000);//���� ���ӱ��� 1�� ���
	}

	if (!fileServerConnectFlag) {//���ϼ��� ���ῡ �����Ͽ��� ���
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: File Server Not Online!!");
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
	printDebugMsg(DC_INFO, errorLevel, "Server Listening at %s:%d", "NOT IMPLEMENTED", 0);

	//Ŭ���̾�Ʈ ���� ���
	while (1) {
		int szClntAddr = sizeof(clientAddr);
		hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &szClntAddr);

		if (hClientSock == INVALID_SOCKET) {//Ŭ���̾�Ʈ ���� ���н�
			printDebugMsg(DC_ERROR, errorLevel, "Client Accept Fail");
			break;
		}

		if (clientCount >= MAX_CON) {//���� ���� ���޽�
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
		printDebugMsg(DC_INFO, errorLevel, "Connection Limit: %d / %d", clientCount, MAX_CON);
		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, clientHandler, (void*)&clientInfo, 0, NULL);//������ �����Ͽ� �ѱ�
		printDebugMsg(DC_INFO, errorLevel, "Client Connected: %s", clientInfo.clientIP);

	}

	closesocket(hServSock);//���� ����� ���� ����
	WSACleanup();//���̺귯�� ����
	printDebugMsg(1, errorLevel, "Server Terminated");
	system("pause");
	return 0;
}