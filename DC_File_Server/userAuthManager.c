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
		//���������� ���� ���� => ���� ��⿭�� �߰�
		memcpy(authWaitList[authWaitCount].authKey, AuthUser.Data.UserFileServerAuthKey, 32);//add authkey to waiting list
		authWaitList[authWaitCount].authWaitTime = (unsigned int)time(NULL); //save auth start time => prevent infinite auth wait
		authWaitList[authWaitCount++].userUID = AuthUser.Data.userUID;
		AuthUserResp.Data.statusCode = 1;
	}

	sendData(hClientSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	return;
}

void procFileLogin(SOCKET hClientSock) {
	//��Ŷ�� 0���� �ʱ�ȭ�Ѵ�.
	cf_LoginFile LoginFile;
	fc_LoginFileResp LoginFileResp;
	memset(&LoginFile, 0, sizeof(cf_LoginFile));
	memset(&LoginFileResp, 0, sizeof(fc_LoginFileResp));

	recvData(hClientSock, LoginFile.buf + 4, sizeof(cf_LoginFile) - 4, 0);
	
	//��Ŷ ��ȯ
	LoginFile.Data.opCode = ntohl(LoginFile.Data.opCode);
	LoginFile.Data.dataLen = ntohl(LoginFile.Data.dataLen);

	//��Ŷ ������ ����
	LoginFileResp.Data.statusCode = 0;

	//���� ��⿭���� ���� ��û ���� Ű �˻�
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < authWaitCount; i++) {
		if (!memcmp(LoginFile.Data.UserFileServerAuthKey, authWaitList[i].authKey, 32)) {//��ġ�ϴ� Ű ���� ���� ���
			for (int j = 0; i < clientCount; j++) {
				if (hClientSock == hClientSocks[j]) {
					memcpy(sessionList[j].authKey, LoginFile.Data.UserFileServerAuthKey, 32); //copy generated sessionKey
					sessionList[j].userUID = authWaitList[i].userUID;
					memcpy(sessionList[j].directory, "/", 2);//�ʱ� ���丮�� ��Ʈ�� ����
					authWaitList[i].authWaitTime = 0; //GC�� ������ �Ϸ�� ����� �ڵ����� �����ϵ���
					LoginFileResp.Data.statusCode = 1;//���� ����
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