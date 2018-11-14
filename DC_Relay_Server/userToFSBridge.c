#include "RelayServer.h"

void procFileServerConnReq(SOCKET hClientSock) {

	//if (!checkStatus(hFileSrvSock)) {
	//	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Lost File Server Connection");
	//	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
	//	system("pause");
	//	exit(1);
	//}

	//allocate memory and init
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	//receive remaining packet
	recvData(hClientSock, FileSrvConReq.buf, sizeof(cs_FileSrvConReq) - 4, 0);

	//convert to host specific
	FileSrvConReq.Data.opCode = OP_CS_FILESRVCONNREQ;
	FileSrvConReq.Data.dataLen = ntohl(FileSrvConReq.Data.dataLen);

	//construct packet
	FileSrvConResp.Data.opCode = htonl(OP_SC_FILESRVCONNRESP);
	FileSrvConResp.Data.dataLen = htonl(sizeof(sc_FileSrvConResp) - 8);

	unsigned char authKey[32];
	GenerateSessionKey(authKey);


	//if(authFSUser()){} else {}

	memcpy(FileSrvConResp.Data.authKey, authKey, 32);
	inet_ntop(AF_INET, &fileSrvAddr.sin_addr, FileSrvConResp.Data.fileSrvAddr, 16);
	FileSrvConResp.Data.fileSrvPort = ntohs(fileSrvAddr.sin_port);
	FileSrvConResp.Data.statusCode = 1;
	

	sendData(hClientSock, FileSrvConResp.buf, sizeof(FileSrvConResp), 0);

	return;
}
