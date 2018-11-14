#include "RelayServer.h"

void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *fileSrvAddr) {

	*hFileSrvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (*hFileSrvSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Invalid Socket");
		return;
	}

	int err = (connect(*hFileSrvSock, (SOCKADDR*)fileSrvAddr, sizeof(*fileSrvAddr)) == SOCKET_ERROR);
	if (err) //생성된 소켓으로 서버에 연결
	{
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FS Connection Failled: %d", WSAGetLastError());
		return;
	}

	sf_RegisterFileServer RegisterFileServer;
	fs_RegisterFileServerResp RegisterFileServerResp;
	memset(&RegisterFileServer, 0, sizeof(sf_RegisterFileServer));
	memset(&RegisterFileServerResp, 0, sizeof(fs_RegisterFileServerResp));

	RegisterFileServer.Data.opCode = htonl(OP_SF_REGISTERFILESERVER);
	RegisterFileServer.Data.dataLen = htonl(0);

	sendData(*hFileSrvSock, RegisterFileServer.buf, sizeof(sf_RegisterFileServer), 0);

	recvData(*hFileSrvSock, RegisterFileServerResp.buf, sizeof(fs_RegisterFileServerResp), 0);

	//convert to host specific endian
	RegisterFileServerResp.Data.opCode = ntohl(RegisterFileServerResp.Data.opCode);
	RegisterFileServerResp.Data.dataLen = ntohl(RegisterFileServerResp.Data.dataLen);
	
	if (RegisterFileServerResp.Data.statusCode == 1)
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Successfully Connected and Registered to File Server");
	else {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Failled to Register");
		return;
	}

	//change global flag
	fileServerConnectFlag = 1;

	return;
};