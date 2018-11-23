/*
Copyright (C) 2018 S.H.Kim (soohyunkim@kw.ac.kr)
Copyright (C) 2018 K.J Choi (chlrhkdwls99@naver.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/**
	@file userAuthManager.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief ����� ���� ���� �Լ� ����
*/

#include "FileServer.h"

/**
	@fn void procAddUserAuthWaitList(SOCKET hClientSock)
	@brief �߰輭���κ����� ���� ������û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock �߰輭�� ����� ����
*/
void procAddUserAuthWaitList(SOCKET hClientSock) {
	//��Ŷ�� ���� ���� �Ҵ� �� �ʱ�ȭ
	sf_AuthUser AuthUser;
	fs_AuthUserResp AuthUserResp;
	memset(&AuthUser, 0, sizeof(sf_AuthUser));
	memset(&AuthUserResp, 0, sizeof(fs_AuthUserResp));

	//�����κ��� ���� ������ ������
	recvData(hClientSock, AuthUser.buf + 4, sizeof(AuthUser) - 4, 0);

	//ȣ��Ʈ ��������� ����
	AuthUser.Data.opCode = ntohl(AuthUser.Data.opCode);
	AuthUser.Data.dataLen = ntohl(AuthUser.Data.dataLen);
	AuthUser.Data.userUID = ntohl(AuthUser.Data.userUID);

	//���� ��Ŷ ����
	AuthUserResp.Data.opCode = htonl(OP_FS_AUTHUSERRESP);
	AuthUserResp.Data.dataLen = htonl(sizeof(AuthUserResp) - 8);

	if (authWaitCount+clientCount >= maxConnection) {//���� ���� ��� + ���� ���� �ִ� ���Ӽ��� �ʰ��ϸ�
		AuthUserResp.Data.statusCode = 0;//���� �ź�
		printDebugMsg(DC_WARN, errorLevel, "WARN: Auth Waiting List and Server Connection is FULL.");
	}
	else {
		//���������� ���� ���� => ���� ��⿭�� �߰�
		memcpy(authWaitList[authWaitCount].authKey, AuthUser.Data.UserFileServerAuthKey, 32);//add authkey to waiting list
		authWaitList[authWaitCount].authWaitTime = (unsigned int)time(NULL); //save auth start time => prevent infinite auth wait
		authWaitList[authWaitCount++].userUID = AuthUser.Data.userUID;
		AuthUserResp.Data.statusCode = 1;//���� ���
	}

	//���� ����
	sendData(hClientSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	return;
}

/**
	@fn void procFileLogin(SOCKET hClientSock)
	@brief Ŭ���̾�Ʈ�� ���ϼ��� �α��� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procFileLogin(SOCKET hClientSock) {
	//��Ŷ �Ҵ� �� 0���� �ʱ�ȭ
	cf_LoginFile LoginFile;
	fc_LoginFileResp LoginFileResp;
	memset(&LoginFile, 0, sizeof(cf_LoginFile));
	memset(&LoginFileResp, 0, sizeof(fc_LoginFileResp));

	//���� ���� �� �޾ƿ�
	recvData(hClientSock, LoginFile.buf + 4, sizeof(cf_LoginFile) - 4, 0);
	
	//ȣ��Ʈ ��������� ��ȯ
	LoginFile.Data.opCode = ntohl(LoginFile.Data.opCode);
	LoginFile.Data.dataLen = ntohl(LoginFile.Data.dataLen);

	//��Ŷ ������ ����
	LoginFileResp.Data.statusCode = 0;//�ϴ� 0����

	int userUID = 0;

	//���� ��⿭���� ���� ��û ���� Ű �˻�
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < authWaitCount; i++) {
		if (!memcmp(LoginFile.Data.UserFileServerAuthKey, authWaitList[i].authKey, 32)) {//��ġ�ϴ� Ű ���� ���� ���
			for (int j = 0; j < clientCount; j++) {
				if (hClientSock == hClientSocks[j]) {
					memcpy(sessionList[j].authKey, LoginFile.Data.UserFileServerAuthKey, 32); //copy generated sessionKey
					sessionList[j].userUID = authWaitList[i].userUID;
					authWaitList[i].authWaitTime = 0; //GC�� ������ �Ϸ�� ����� �ڵ����� �����ϵ���
					LoginFileResp.Data.statusCode = 1;//���� ����
					userUID = sessionList[j].userUID;
					break;
				}
			}
			break;
		}
	}
	ReleaseMutex(hMutex);

	//��Ŷ ����
	LoginFileResp.Data.opCode = htonl(OP_FC_LOGINFILERESP);
	LoginFileResp.Data.dataLen = htonl(sizeof(fc_LoginFileResp) - 8);

	//���� ����
	sendData(hClientSock, LoginFileResp.buf, sizeof(fc_LoginFileResp), 0);

	printDebugMsg(DC_INFO, errorLevel, "Auth Done.");

	return;
}