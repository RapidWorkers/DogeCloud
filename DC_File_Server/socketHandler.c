#include "FileServer.h"

unsigned int WINAPI clientHandler(void* clientInfo) {
	SOCKET hClientSock = *(((DC_SOCK_INFO*)clientInfo)->hSocket);
	char clientIP[16];
	memcpy_s(clientIP, 16, ((DC_SOCK_INFO*)clientInfo)->clientIP, 16);

	unsigned char opCodeBuffer[4];
	memset(opCodeBuffer, 0, 4);

	while (recvRaw(hClientSock, opCodeBuffer, 4, 0)) {
		unsigned long opCode;
		memcpy(&opCode, opCodeBuffer, 4);
		opCode = ntohl(opCode);
		packetHandler(hClientSock, clientIP, opCode);
	}//loop while socket is alive

	printDebugMsg(1, DC_ERRORLEVEL, "Connection Closed");
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

void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode) {
	if (hClientSock == INVALID_SOCKET) return;

	printDebugMsg(DC_DEBUG, DC_ERRORLEVEL, "Got OpCode: %d", opCode);
	switch (opCode) {
	case OP_SF_REGISTERFILESERVER:
		procRegisterFileServer(hClientSock);
		break;
	default:
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Unknown Packet: %s", clientIP);
		break;
	}
}