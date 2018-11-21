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
	@file userToFSBridge.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief �����-���ϼ��� ���� �Լ�
*/

#include "RelayServer.h"

/**
	@fn void procFileServerConnReq(SOCKET hClientSock)
	@brief ������� ���ϼ��� �����û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����� ����� ����
*/
void procFileServerConnReq(SOCKET hClientSock) {

	//if (!checkStatus(hFileSrvSock)) {
	//	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Lost File Server Connection");
	//	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
	//	system("pause");
	//	exit(1);
	//}

	//��Ŷ ���� �� �ʱ�ȭ
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	//�����ִ� ������ �޾ƿ�
	recvData(hClientSock, FileSrvConReq.buf, sizeof(cs_FileSrvConReq) - 4, 0);

	//ȣ��Ʈ Ư�� ��������� ��ȯ
	FileSrvConReq.Data.opCode = OP_CS_FILESRVCONNREQ;
	FileSrvConReq.Data.dataLen = ntohl(FileSrvConReq.Data.dataLen);

	//��Ŷ ����
	FileSrvConResp.Data.opCode = htonl(OP_SC_FILESRVCONNRESP);
	FileSrvConResp.Data.dataLen = htonl(sizeof(sc_FileSrvConResp) - 8);
	
	unsigned char authKey[32];
	unsigned long userUID;

	//������ ����Ű ����
	GenerateSessionKey(authKey);

	//������������ UID ������
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			userUID = sessionList[i].userUID;
			break;
		}
	}
	ReleaseMutex(hMutex);

	int authResult = 0;

	//���ϼ��� ���� ó��
	authFSUser(authKey, userUID, &authResult);
	if(authResult){//���� ������
		memcpy(FileSrvConResp.Data.authKey, authKey, 32);//��Ŷ�� ����Ű ����
		inet_ntop(AF_INET, &fileSrvAddr.sin_addr, FileSrvConResp.Data.fileSrvAddr, 16);
		FileSrvConResp.Data.fileSrvPort = ntohs(fileSrvAddr.sin_port);
		FileSrvConResp.Data.statusCode = 1;
	} else {//���� ���н�
		FileSrvConResp.Data.statusCode = 0;
	}

	//��Ŷ ����
	sendData(hClientSock, FileSrvConResp.buf, sizeof(FileSrvConResp), 0);

	return;
}

/**
	@fn void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag)
	@brief ����ڸ� ���ϼ����� ������Ŵ
	@author �۸۾ƾ߿��غ�
	@param *authKey ����Ű ���ڿ�
	@param userUID ���� UID
	@param resultFlag ��� �÷���
*/
void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag) {
	//��Ŷ ���� �� �ʱ�ȭ
	sf_AuthUser AuthUser;
	fs_AuthUserResp AuthUserResp;
	memset(&AuthUser, 0, sizeof(sf_AuthUser));
	memset(&AuthUserResp, 0, sizeof(fs_AuthUserResp));

	//��Ŷ ������ ����
	AuthUser.Data.opCode = htonl(OP_SF_AUTHUSER);
	AuthUser.Data.dataLen = htonl(sizeof(AuthUser) - 8);
	AuthUser.Data.userUID = htonl(userUID);
	memcpy(AuthUser.Data.UserFileServerAuthKey, authKey, 32);

	//��Ŷ ����
	sendData(hFileSrvSock, AuthUser.buf, sizeof(sf_AuthUser), 0);
	//���� ����
	recvData(hFileSrvSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	//ȣ��Ʈ Ư�� ��������� ��ȯ
	AuthUserResp.Data.opCode = ntohl(AuthUserResp.Data.opCode);
	AuthUserResp.Data.dataLen = ntohl(AuthUserResp.Data.dataLen);
	
	//�������� ��ȯ
	if (AuthUserResp.Data.statusCode == 1)
		*resultFlag = 1;
	else
		*resultFlag = 0;

	return;
}