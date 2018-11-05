#include "FileServer.h"

unsigned int WINAPI clientHandler(void* socket) {
	SOCKET hClientSock = *((SOCKET*)socket);

	//TODO: create some global

	int strLen = 0;
	unsigned char opCodeBuffer[4];
	memset(opCodeBuffer, 0, 4);

	while (recvRaw(hClientSock, opCodeBuffer, 4, 0)) {
		unsigned long opCode;
		memcpy(&opCode, opCodeBuffer, 4);
		opCode = ntohl(opCode);
		packetHandler(hClientSock, opCode);
	}//loop while socket is alive

	printDebugMsg(1, ERLEVEL, "Connection Closed");
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clientCount; i++) {
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

void packetHandler(SOCKET hClientSock, unsigned long opCode) {
	if (hClientSock == INVALID_SOCKET) return;
	printf("Got OpCode: %d", opCode);
	switch (opCode) {
	default:
		printDebugMsg(3, ERLEVEL, "Unknown Packet");
		//printDebugMsg(3, ERLEVEL, &opCode);
		break;
	}
}