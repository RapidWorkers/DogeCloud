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
	@var SOCKET hClientSocks[MAX_CON]
	���� ����� ����ü �迭
*/
SOCKET hClientSocks[MAX_CON];
/**
	@var DC_FILE_SESSION sessionList[MAX_CON]
	���� ����� ����ü �迭
*/
DC_FILE_SESSION sessionList[MAX_CON];
/**
	@var DC_AUTHWAIT_LIST authWaitList[MAX_CON]
	���� ��⿭ ���� ����ü �迭
*/
DC_AUTHWAIT_LIST authWaitList[MAX_CON];
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
	@fn int main()
	@brief ���ϼ��� ������
	@author �۸۾ƾ߿��غ�
*/
int main()
{
	/**
		@var WSADATA wsaData
		wsa ���̺귯���� ����ü
	*/
	WSADATA wsaData;
	/**
		@var SOCKET hServSock, hClientSock
		������ ����, Ŭ���̾�Ʈ ���� ó���� ����
	*/
	SOCKET hServSock, hClientSock;
	/**
		@var SOCKADDR_IN servAddr, clientAddr
		���� �ּ�, Ŭ���̾�Ʈ �ּ� ��� ����ü
	*/
	SOCKADDR_IN servAddr, clientAddr;

	/**
		@var HANDLE hThread
		��Ƽ������ �ڵ�
	*/
	HANDLE hThread = NULL;

	//unsigned int threadID;

	/**
		@var HANDLE hGCThread
		������ �ݷ��Ϳ� ��Ƽ������ �ڵ�
	*/
	HANDLE hGCThread = NULL;

	//���ؽ� ����
	hMutex = CreateMutex(NULL, FALSE, NULL);
	//DWORD dwErrorCode = 0;

	printProgramInfo();

	//�����ͺ��̽� ���� �а� ����
	readMySQLConfig(&serverInfo);
	sqlInit(&sqlHandle, serverInfo);

	//���� �ʱ�ȭ ����
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {//WSA Startup
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "WSAStartup Fail");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);//���� ���� �ʱ�ȭ
	if (hServSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Invalid Server Socket");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);
	}

	//���� �ּ� ����
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(BIND_PORT);

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) {//���ε�
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Bind Fail");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);
	}

	if (listen(hServSock, 5) == SOCKET_ERROR) {//���� ������ ����
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Listen Fail");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Server Started");
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Server Listening at %s:%d", "NOT IMPLEMENTED", 0);

	//������ �ݷ��� ����
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Starting Garbage Collector");
	hGCThread = (HANDLE)_beginthreadex(NULL, 0, waitingListGC, NULL, 0, NULL);
	if (hGCThread == NULL) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "GC Start Fail");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);
	}
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Done Starting Garbage Collector");

	//Ŭ���̾�Ʈ ���� ���
	while (1) {
		int szClntAddr = sizeof(clientAddr);
		hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &szClntAddr);

		if (hClientSock == INVALID_SOCKET) {//Ŭ���̾�Ʈ ���� ���н�
			printDebugMsg(3, DC_ERRORLEVEL, "Client Accept Fail");
			break;
		}

		WaitForSingleObject(hMutex, INFINITE);
		hClientSocks[clientCount++] = hClientSock;
		/**
			@var DC_SOCK_INFO clientInfo
			Ŭ���̾�Ʈ ���� ���� ����ü
		*/
		DC_SOCK_INFO clientInfo;
		clientInfo.hSocket = &hClientSock;
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientInfo.clientIP, 16);//Ŭ���̾�Ʈ ip ���ڿ��� ��ȯ
		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, clientHandler, (void*)&clientInfo, 0, NULL);//������ �����Ͽ� �ѱ�
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Client Connected: %s", clientInfo.clientIP);

	}

	closesocket(hServSock);//���� ����� ���� ����
	WSACleanup();//���̺귯�� ����
	printDebugMsg(1, DC_ERRORLEVEL, "Server Terminated");
	system("pause");
	return 0;
}