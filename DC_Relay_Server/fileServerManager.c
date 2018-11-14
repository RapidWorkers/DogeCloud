#include "RelayServer.h"

void procRegisterFileServer(SOCKET hClientSock) {
	//allocate and init memory for packet
	fs_RegisterFileServer RegisterFileServer;
	sf_RegisterFileServerResp RegisterFileServerResp;
	memset(&RegisterFileServer, 0, sizeof(fs_RegisterFileServer));
	memset(&RegisterFileServerResp, 0, sizeof(sf_RegisterFileServerResp));

	//receive remaining packet
	if (!recvRaw(hClientSock, RegisterFileServer.buf, sizeof(RegisterFileServer)-4, 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());
		return;
	}

	//convert big endian to host specific
	RegisterFileServer.Data.opCode = OP_FS_REGISTERFILESERVER;
	RegisterFileServer.Data.dataLen = ntohl(RegisterFileServer.Data.dataLen);

	//construct response packet
	RegisterFileServerResp.Data.opCode = htonl(OP_SF_REGISTERFILESERVERRESP);
	RegisterFileServerResp.Data.dataLen = htonl(sizeof(sf_RegisterFileServerResp) - 8);

	if (fileServerRegisterFlag) {
		RegisterFileServerResp.Data.statusCode = 0;//already registerd
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "FileServer Register Denied: %s:%ul", RegisterFileServer.Data.srvAddr, ntohl(RegisterFileServer.Data.srvPort));
	}
	else {
		//set data
		WaitForSingleObject(hMutex, INFINITE);
		memcpy(fileServerAddr, RegisterFileServer.Data.srvAddr, 16);
		fileServerPort = RegisterFileServer.Data.srvPort;
		fileServerRegisterFlag = 1;
		//currentStatus[clientCount] = 254; //means this client is fileserver
		printDebugMsg(DC_INFO, DC_ERRORLEVEL, "FileServer Registered: %s:%ul", fileServerAddr, ntohl(fileServerPort));
		ReleaseMutex(hMutex);
	}
	
	//send result
	if (!sendRaw(hClientSock, RegisterFileServerResp.buf, sizeof(RegisterFileServerResp), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());
		return;
	}

	return;
}

void procFileServerConnReq(SOCKET hClientSock) {
	//allocate memory and init
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	//receive remaining packet
	if (!recvRaw(hClientSock, FileSrvConReq.buf, sizeof(cs_FileSrvConReq) - 4, 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());
		return;
	}

	//convert to host specific
	FileSrvConReq.Data.opCode = OP_CS_FILESRVCONNREQ;
	FileSrvConReq.Data.dataLen = ntohl(FileSrvConReq.Data.dataLen);

	//construct packet
	FileSrvConResp.Data.opCode = htonl(OP_SC_FILESRVCONNRESP);
	FileSrvConResp.Data.dataLen = htonl(sizeof(sc_FileSrvConResp) - 8);
	if (fileServerRegisterFlag) {
		FileSrvConResp.Data.statusCode = 1;
		memcpy(FileSrvConResp.Data.fileSrvAddr, fileServerAddr, 16);
		FileSrvConResp.Data.fileSrvPort = fileServerPort;
	}
	else {
		FileSrvConResp.Data.statusCode = 0;
	}

	if (!sendRaw(hClientSock, FileSrvConResp.buf, sizeof(FileSrvConResp), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());
		return;
	}

	return;
}