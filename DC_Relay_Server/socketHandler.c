#include "RelayServer.h"

unsigned int WINAPI clientHandler(void* socket) {
	SOCKET hClientSock = *((SOCKET*)socket);
	
	//create session related
	unsigned char sessionKey[32];
	unsigned char loginStatus;

	int strLen = 0;
	unsigned char opCodeBuffer[4];
	memset(opCodeBuffer, 0, 4);
	
	while(strLen = recv(hClientSock, opCodeBuffer, 4, 0) > 0){
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
	case OP_CS_LOGINSTART:
		printDebugMsg(1, ERLEVEL, "LoginStart OpCode");
		procLoginStart(hClientSock);
		break;
	case OP_CS_LOGINACCOUNTDATA:
		printDebugMsg(1, ERLEVEL, "LoginAccountData OpCode");
		procLoginAccountData(hClientSock);
		break;
	case OP_CS_LOGOUTSTART:
		printDebugMsg(1, ERLEVEL, "LogoutStart OpCode");
		procLogout(hClientSock);
		break;

	case 250: //FILE UPLOAD DEMO
		printDebugMsg(1, ERLEVEL, "File Upload Demo Mode");
		procFileUpDemo(hClientSock);
		break;
	case 251: //FILE DOWNLOAD DEMO
		printDebugMsg(1, ERLEVEL, "File Download Demo Mode");
		procFileDownDemo(hClientSock);
		break;

	default:
		printDebugMsg(3, ERLEVEL, "Unknown Packet");
		//printDebugMsg(3, ERLEVEL, &opCode);
		break;
	}
}