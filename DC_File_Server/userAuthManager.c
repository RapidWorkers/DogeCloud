#include "FileServer.h"

void procAddUserAuthWaitList(SOCKET hClientSock) {
	sf_AuthUser AuthUser;
	fs_AuthUserResp AuthUserResp;
	memset(&AuthUser, 0, sizeof(sf_AuthUser));
	memset(&AuthUserResp, 0, sizeof(fs_AuthUserResp));

	recvData(hClientSock, AuthUser.buf + 4, sizeof(AuthUser) - 4, 0);

	//convert to host specific endian
	AuthUser.Data.opCode = ntohl(AuthUser.Data.opCode);
	AuthUser.Data.dataLen = ntohl(AuthUser.Data.dataLen);


	//construct response packet
	AuthUserResp.Data.opCode = htonl(OP_FS_AUTHUSERRESP);
	AuthUserResp.Data.dataLen = htonl(sizeof(AuthUserResp) - 8);

	if (authWaitCount >= MAX_CON) {//waiting list limit reached
		AuthUserResp.Data.statusCode = 0;
		printDebugMsg(DC_WARN, DC_ERRORLEVEL, "WARN: Auth Waiting List FULL.");
	}
	else {
		memcpy(authWait[authWaitCount], AuthUser.Data.UserFileServerAuthKey, 32);//add authkey to waiting list
		authWaitTime[authWaitCount++] = (unsigned int)time(NULL); //save auth start time => prevent infinite auth wait
		AuthUserResp.Data.statusCode = 1;
	}

	sendData(hClientSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	return;
}