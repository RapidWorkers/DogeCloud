#include "RelayServer.h"

unsigned int WINAPI clientHandler(void* clientInfo) {
	SOCKET hClientSock = *(((DC_SOCK_INFO*)clientInfo)->hSocket);
	char clientIP[16];
	memcpy_s(clientIP, 16, ((DC_SOCK_INFO*)clientInfo)->clientIP, 16);

	unsigned char opCodeBuffer[4];
	memset(opCodeBuffer, 0, 4);
	
	while(recvRaw(hClientSock, opCodeBuffer, 4, 0)){
		unsigned long opCode;
		memcpy(&opCode, opCodeBuffer, 4);
		opCode = ntohl(opCode);
		packetHandler(hClientSock, clientIP, opCode);
	}//loop while socket is alive

	printDebugMsg(1, DC_ERRORLEVEL, "Connection Closed");

	//delete data of disconnected client
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			while (i++ < clientCount - 1) {
				hClientSocks[i] = hClientSocks[i + 1];
				memcpy(sessionKey[i], sessionKey[i + 1], 32);
			}
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
	case OP_CS_LOGINSTART:
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "LoginStart: %s", clientIP);
		procLoginStart(hClientSock);
		break;

	case OP_CS_LOGOUTSTART:
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "LogoutStart: %s", clientIP);
		procLogout(hClientSock);
		break;

	case OP_CS_REGISTERSTART:
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "RegisterStart: %s", clientIP);
		procRegisterStart(hClientSock);
		break;

	case OP_CS_FILESRVCONNREQ:
		break;

	//fileserver connection
	case OP_FS_REGISTERFILESERVER:
		break;

	//case 250: //FILE UPLOAD DEMO
	//	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "File Upload Demo Mode", clientIP);
	//	procFileUpDemo(hClientSock);
	//	break;

	//case 251: //FILE DOWNLOAD DEMO
	//	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "File Download Demo Mode", clientIP);
	//	procFileDownDemo(hClientSock);
	//	break;

	default:
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Unknown Packet: %s", clientIP);
		//printDebugMsg(3, DC_ERRORLEVEL, &opCode);
		break;
	}
}