#include "FileServer.h"

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

	//init sockets
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)//WSA Startup
		printDebugMsg(3, ERLEVEL, "Init Sock fail");
	hServSock = socket(PF_INET, SOCK_STREAM, 0);//init server socket
	if (hServSock == INVALID_SOCKET)
		printDebugMsg(3, ERLEVEL, "Invalid Sock");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(BIND_PORT);

	if (bind(hServSock, &servAddr, sizeof(servAddr)))
		printDebugMsg(3, ERLEVEL, "Bind Fail");

	if (listen(hServSock, 5) == SOCKET_ERROR)
		printDebugMsg(3, ERLEVEL, "Listen Fail");

	printDebugMsg(1, ERLEVEL, "Server Started");

	while (1) {
		int szClntAddr = sizeof(clientAddr);
		hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &szClntAddr);
		if (hClientSock == INVALID_SOCKET)
			printDebugMsg(3, ERLEVEL, "Client Accept Fail");
		WaitForSingleObject(hMutex, INFINITE);
		hClientSocks[clientCount++] = hClientSock;
		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, clientHandler, (void*)&hClientSock, 0, NULL);
		printDebugMsg(1, ERLEVEL, "Client Connected");
		char ipAddr[16];
		inet_ntop(AF_INET, &clientAddr.sin_addr, ipAddr, 16);
		printf("%s\n", ipAddr);
	}
	closesocket(hServSock);
	WSACleanup();
	printDebugMsg(1, ERLEVEL, "Server Terminated");
	system("pause");
	return 0;
}