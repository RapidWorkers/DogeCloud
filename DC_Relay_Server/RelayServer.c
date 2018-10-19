#include "RelayServer.h"

unsigned int WINAPI clientHandler(void* socket) {
	SOCKET hClientSock = *((SOCKET*)socket);
	
	int strLen = 0;
	BYTE opCodeBuffer;

	recv(hClientSock, &opCodeBuffer, 1, 0);//first byte of packet always contains operation code
	packetHandler(&hClientSock, opCodeBuffer);


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

void packetHandler(SOCKET *socket, BYTE opCode) {
	SOCKET hClientSock = *socket;
	if (socket == NULL) return;
	switch (opCode) {
	case 139:
		printDebugMsg(1, ERLEVEL, "Received LoginStart OpCode (Currently in-dev)");
		char tmp_buf[sizeof(cs_LoginStart)];
		*tmp_buf = opCode;
		recv(hClientSock, tmp_buf+1, sizeof(cs_LoginStart)-1, 0);

		sc_LoginDoneResp LoginDoneResp;
		LoginDoneResp.opCode = 134;
		LoginDoneResp.statusCode = 1;
		for (int i = 0; i < 32; i++)
			LoginDoneResp.sessionKey[i] = i;

		char buf[sizeof(sc_LoginDoneResp)];
		memcpy(buf, &LoginDoneResp, sizeof(LoginDoneResp));
		send(hClientSock, buf, sizeof(buf), 0);
		printDebugMsg(1, ERLEVEL, "Sent LoginDoneResp to Client");
		break;
	default:
		printDebugMsg(3, ERLEVEL, "Unknown Packet");
		printDebugMsg(3, ERLEVEL, &opCode);
		break;
	}
}