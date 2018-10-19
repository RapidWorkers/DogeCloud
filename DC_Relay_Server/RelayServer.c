#include "RelayServer.h"

unsigned int WINAPI clientHandler(void* socket) {
	SOCKET hClientSock = *((SOCKET*)socket);
	
	int strLen = 0, i;
	char msg[] = "Hello World!";

	WaitForSingleObject(hMutex, INFINITE);
	send(hClientSock, msg, sizeof(msg), 0);
	ReleaseMutex(hMutex);

	WaitForSingleObject(hMutex, INFINITE);
	for (i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			while (i++ < clientCount - 1)
				hClientSocks[i] = hClientSocks[i + 1];
			break;
		}
	}
	clientCount--;
	ReleaseMutex(hMutex);
	closesocket(hClientSock);

	return 0;
}
