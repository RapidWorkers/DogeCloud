#include "RelayServer.h"

//Mutex init
HANDLE hMutex;

//create Client Sock array
SOCKET hClientSocks[MAX_CON];
int clientCount;

int main()
{
	//Winsock Structures init
	WSADATA wsaData;
	SOCKET hServSock, hClientSock;
	SOCKADDR_IN servAddr, clientAddr;

	//Multithread init
	HANDLE hThread = NULL;
	unsigned int threadID;

	//Create Mutex;
	hMutex = CreateMutex(NULL, FALSE, NULL);
	DWORD dwErrorCode = 0;

	printProgramInfo();

	//MariaDB Handle and init connection
	MYSQL_SERVER serverInfo = { MYSQL_ADDR, MYSQL_PORT, MYSQL_USER, MYSQL_PASS, MYSQL_DBASE };
	MYSQL sqlHandle;
	sqlInit(&sqlHandle, serverInfo);
	
	//init sockets
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//WSA Startup
		printDebugMsg(3, DC_ERRORLEVEL,"Init Sock fail");
	hServSock = socket(PF_INET, SOCK_STREAM, 0);//init server socket
	if (hServSock == INVALID_SOCKET)
		printDebugMsg(3, DC_ERRORLEVEL, "Invalid Sock");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(BIND_PORT);

	if (bind(hServSock, &servAddr, sizeof(servAddr))) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Bind Fail");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
	}
		

	if (listen(hServSock, 5) == SOCKET_ERROR) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Listen Fail");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Server Started");

	while (1) {
		int szClntAddr = sizeof(clientAddr);
		hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &szClntAddr);

		if (hClientSock == INVALID_SOCKET) {
			printDebugMsg(3, DC_ERRORLEVEL, "Client Accept Fail");
			break;
		}

		WaitForSingleObject(hMutex, INFINITE);
		hClientSocks[clientCount++] = hClientSock;
		DC_CLIENT_INFO clientInfo;
		clientInfo.hSocket = &hClientSock;
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientInfo.clientIP, 16);
		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, clientHandler, (void*)&clientInfo, 0, NULL);
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Client Connected: %s", clientInfo.clientIP);

	}

	closesocket(hServSock);
	WSACleanup();
	printDebugMsg(1, DC_ERRORLEVEL, "Server Terminated");
	system("pause");
	return 0;
}