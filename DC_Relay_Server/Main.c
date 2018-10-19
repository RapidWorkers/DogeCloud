#include <stdio.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

#define DEVELOPEMENT_MODE

//FOR TESTING ONLY
#ifdef DEVELOPEMENT_MODE
#define BUF_SIZE 2048
#define QUEUE_SIZE 10
#define BIND_ADDR "127.0.0.1"
#define BIND_PORT 15754
#endif

int main()
{

	WSADATA wsaData;
	SOCKET hServSock, hClntSock;
	SOCKADDR_IN servAddr, clntAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		printf("Error: Init Sock fail");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hServSock == INVALID_SOCKET)
		printf("Error: Invalid Sock");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(BIND_PORT);

	if (bind(hServSock, &servAddr, sizeof(servAddr)))
		printf("Error: Bind Fail");

	if (listen(hServSock, 5) == SOCKET_ERROR)
		printf("Error: Listen Fail");

	int szClntAddr = sizeof(clntAddr);
	hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr);

	if(hClntSock == INVALID_SOCKET)
		printf("Error: Accept Fail");

	send(hClntSock, "Hello World!", 13, 0);
	closesocket(hClntSock);
	closesocket(hServSock);
	WSACleanup();

	return 0;

	printf("Not ready yet :) \n");
	system("pause");
}