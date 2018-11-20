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
	AuthUser.Data.userUID = ntohl(AuthUser.Data.userUID);

	//construct response packet
	AuthUserResp.Data.opCode = htonl(OP_FS_AUTHUSERRESP);
	AuthUserResp.Data.dataLen = htonl(sizeof(AuthUserResp) - 8);

	if (authWaitCount+clientCount >= MAX_CON) {//waiting list & connection limit reached
		AuthUserResp.Data.statusCode = 0;
		printDebugMsg(DC_WARN, DC_ERRORLEVEL, "WARN: Auth Waiting List and Server Connection is FULL.");
	}
	else {
		//전역변수에 변수 세팅 => 인증 대기열에 추가
		memcpy(authWaitList[authWaitCount].authKey, AuthUser.Data.UserFileServerAuthKey, 32);//add authkey to waiting list
		authWaitList[authWaitCount].authWaitTime = (unsigned int)time(NULL); //save auth start time => prevent infinite auth wait
		authWaitList[authWaitCount++].userUID = AuthUser.Data.userUID;
		AuthUserResp.Data.statusCode = 1;
	}

	sendData(hClientSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	return;
}

void procFileLogin(SOCKET hClientSock) {
	//패킷을 0으로 초기화한다.
	cf_LoginFile LoginFile;
	fc_LoginFileResp LoginFileResp;
	memset(&LoginFile, 0, sizeof(cf_LoginFile));
	memset(&LoginFileResp, 0, sizeof(fc_LoginFileResp));

	recvData(hClientSock, LoginFile.buf + 4, sizeof(cf_LoginFile) - 4, 0);
	
	//패킷 변환
	LoginFile.Data.opCode = ntohl(LoginFile.Data.opCode);
	LoginFile.Data.dataLen = ntohl(LoginFile.Data.dataLen);

	//패킷 데이터 설정
	LoginFileResp.Data.statusCode = 0;

	//인증 대기열에서 인증 요청 받은 키 검색
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < authWaitCount; i++) {
		if (!memcmp(LoginFile.Data.UserFileServerAuthKey, authWaitList[i].authKey, 32)) {//일치하는 키 값이 있을 경우
			for (int j = 0; i < clientCount; j++) {
				if (hClientSock == hClientSocks[j]) {
					memcpy(sessionList[j].authKey, LoginFile.Data.UserFileServerAuthKey, 32); //copy generated sessionKey
					sessionList[j].userUID = authWaitList[i].userUID;
					memcpy(sessionList[j].directory, "/", 2);//초기 디렉토리는 루트로 설정
					authWaitList[i].authWaitTime = 0; //GC가 인증이 완료된 목록을 자동으로 제거하도록
					LoginFileResp.Data.statusCode = 1;//인증 성공
					break;
				}
			}
			break;
		}
	}
	ReleaseMutex(hMutex);

	LoginFileResp.Data.opCode = htonl(OP_FC_LOGINFILERESP);
	LoginFileResp.Data.dataLen = htonl(sizeof(fc_LoginFileResp) - 8);

	sendData(hClientSock, LoginFileResp.buf, sizeof(fc_LoginFileResp), 0);

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Auth Done.");

	return;
}