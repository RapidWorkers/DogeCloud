#include "FileServer.h"

void procRegisterFileServer(SOCKET hClientSock) {
	sf_RegisterFileServer RegisterFileServer;
	fs_RegisterFileServerResp RegisterFileServerResp;
	memset(&RegisterFileServer, 0, sizeof(sf_RegisterFileServer));
	memset(&RegisterFileServerResp, 0, sizeof(fs_RegisterFileServerResp));

	recvData(hClientSock, RegisterFileServer.buf+4, sizeof(sf_RegisterFileServer)-4, 0);
	//convert to host specific endian
	RegisterFileServer.Data.opCode = OP_SF_REGISTERFILESERVER;
	RegisterFileServer.Data.dataLen = ntohl(RegisterFileServer.Data.dataLen);

	//TODO: if ip is not allowed, close connection at al.

	//construct packet
	RegisterFileServerResp.Data.opCode = htonl(OP_FS_REGISTERFILESERVERRESP);
	RegisterFileServerResp.Data.dataLen = htonl(0);
	RegisterFileServerResp.Data.statusCode = 1;
	sendData(hClientSock, RegisterFileServerResp.buf, sizeof(fs_RegisterFileServerResp), 0);

	return;
}