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

	int authResult = 0;
	authFSUser(authKey, &authResult);
	if(authResult){
		memcpy(FileSrvConResp.Data.authKey, authKey, 32);
		inet_ntop(AF_INET, &fileSrvAddr.sin_addr, FileSrvConResp.Data.fileSrvAddr, 16);
		FileSrvConResp.Data.fileSrvPort = ntohs(fileSrvAddr.sin_port);
		FileSrvConResp.Data.statusCode = 1;
	} else 
	{
		FileSrvConResp.Data.statusCode = 0;
	}

	sendData(hClientSock, FileSrvConResp.buf, sizeof(FileSrvConResp), 0);

	return;
}

void authFSUser(char* authKey, int *resultFlag) {
	sf_AuthUser AuthUser;
	fs_AuthUserResp AuthUserResp;
	memset(&AuthUser, 0, sizeof(sf_AuthUser));
	memset(&AuthUserResp, 0, sizeof(fs_AuthUserResp));

	//construct packet
	AuthUser.Data.opCode = htonl(OP_SF_AUTHUSER);
	AuthUser.Data.dataLen = htonl(sizeof(AuthUser) - 8);

	sendData(hFileSrvSock, AuthUser.buf, sizeof(sf_AuthUser), 0);

	recvData(hFileSrvSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	//convert to host specific endian
	AuthUserResp.Data.opCode = ntohl(AuthUserResp.Data.opCode);
	AuthUserResp.Data.dataLen = ntohl(AuthUserResp.Data.dataLen);
	
	if (AuthUserResp.Data.statusCode == 1)
		*resultFlag = 1;
	else
		*resultFlag = 0;

	return;
}