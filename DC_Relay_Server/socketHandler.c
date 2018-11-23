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
	@file socketHandler.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief �߰輭�� ���� �ڵ鷯
*/

#include "RelayServer.h"

/**
	@fn unsigned int WINAPI clientHandler(void* clientInfo)
	@brief Ŭ���̾�Ʈ �ڵ鷯
	@author �۸۾ƾ߿��غ�
	@param clientInfo (DC_SOCK_INFO)�� void*�� ĳ������ ������
*/
unsigned int WINAPI clientHandler(void* clientInfo) {
	//������ ����� ���� ĳ����
	SOCKET hClientSock = *(((DC_SOCK_INFO*)clientInfo)->hSocket);
	char clientIP[16];
	memcpy_s(clientIP, 16, ((DC_SOCK_INFO*)clientInfo)->clientIP, 16);

	unsigned long opCode = 0;

	while (recvRaw(hClientSock, (char*)&opCode, 4, 0)) {//������ ���涧���� ���ڵ带 Ŭ���̾�Ʈ���� �޾ƿ�
		opCode = ntohl(opCode);//ȣ��Ʈ ��������� ��ȯ
		packetHandler(hClientSock, clientIP, opCode);//��Ŷ �ڵ鷯�� �ѱ�
	}

	printDebugMsg(1, errorLevel, "Connection Closed");

	//������ ����Ǹ� �����Ǿ��� ���������� ����
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			while (i < clientCount - 1) {
				hClientSocks[i] = hClientSocks[i + 1];
				memcpy(&sessionList[i], &sessionList[i + 1], sizeof(DC_SESSION));
				i++;
			}
			//���� 0���� ���� => ���� ����
			memset(&sessionList[i], 0, sizeof(DC_SESSION));
			memset(&hClientSocks[i], 0, sizeof(SOCKET));
			break;
		}
	}
	clientCount--;//Ŭ���̾�Ʈ ī��Ʈ ����
	printDebugMsg(DC_INFO, errorLevel, "Connection Limit: %d / %d", clientCount, maxConnection);
	ReleaseMutex(hMutex);

	closesocket(hClientSock);//���� ����
	return 0;//������ ����
}

/**
	@fn void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode)
	@brief ��Ŷ �ڵ鷯
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ�� ����� ����
	@param *clientIP Ŭ���̾�Ʈ IP
	@param opCode ��Ŷ opCode
*/
void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode) {
	if (hClientSock == INVALID_SOCKET) return;//��ȿ���� ���� �����̸� �׳� ����

	printDebugMsg(DC_DEBUG, errorLevel, "Got OpCode: %d", opCode);
	switch (opCode) {//opCode�� ���� ����
	case OP_CS_LOGINSTART:
		printDebugMsg(DC_INFO, errorLevel, "LoginStart: %s", clientIP);
		procLoginStart(hClientSock);
		break;
	case OP_CS_LOGOUTSTART:
		printDebugMsg(DC_INFO, errorLevel, "LogoutStart: %s", clientIP);
		procLogout(hClientSock);
		break;
	case OP_CS_REGISTERSTART:
		printDebugMsg(DC_INFO, errorLevel, "RegisterStart: %s", clientIP);
		procRegisterStart(hClientSock);
		break;
	case OP_CS_FILESRVCONNREQ:
		printDebugMsg(DC_INFO, errorLevel, "FileServerConnReq: %s", clientIP);
		procFileServerConnReq(hClientSock);
		break;
	case OP_CS_DOWNLOADUSERINFOREQ:
		printDebugMsg(DC_INFO, errorLevel, "DownloadPersonalDBFile Req: %s", clientIP);
		procDownloadPersonalDBFile(hClientSock);
		break;
	case OP_CS_PERSONALDBEDITDONE:
		printDebugMsg(DC_INFO, errorLevel, "UploadPersonalDBFile Req: %s", clientIP);
		procUploadPersonalDBFile(hClientSock);
		break;
	default:
		printDebugMsg(DC_ERROR, errorLevel, "Unknown Packet: %s", clientIP);
		break;
	}
}