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
	@file fileServerRegister.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief ���ϼ��� ���ó�� �Լ� ����
*/

#include "RelayServer.h"

/**
	@fn void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *FileServAddr)
	@brief ���ϼ��� ����
	@author �۸۾ƾ߿��غ�
	@param *hFileSrvSock ���ϼ��� ����� ����
	@param *FileServAddr �߰� ���� ����� �ּ� ����ü �ּ�
*/
void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *fileSrvAddr) {
	//��Ŷ ���� �� �ʱ�ȭ
	sf_RegisterFileServer RegisterFileServer;
	fs_RegisterFileServerResp RegisterFileServerResp;
	memset(&RegisterFileServer, 0, sizeof(sf_RegisterFileServer));
	memset(&RegisterFileServerResp, 0, sizeof(fs_RegisterFileServerResp));

	//���� ����
	*hFileSrvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (*hFileSrvSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, errorLevel, "Invalid Socket");
		return;
	}

	//���� ����
	int err = (connect(*hFileSrvSock, (SOCKADDR*)fileSrvAddr, sizeof(*fileSrvAddr)) == SOCKET_ERROR);
	if (err) //������ �������� ������ ����
	{
		printDebugMsg(DC_ERROR, errorLevel, "FS Connection Failled: %d", WSAGetLastError());
		return;
	}

	//��Ŷ ����
	RegisterFileServer.Data.opCode = htonl(OP_SF_REGISTERFILESERVER);
	RegisterFileServer.Data.dataLen = htonl(0);

	//��� ��Ŷ ����
	sendData(*hFileSrvSock, RegisterFileServer.buf, sizeof(sf_RegisterFileServer), 0);
	//���� ����
	recvData(*hFileSrvSock, RegisterFileServerResp.buf, sizeof(fs_RegisterFileServerResp), 0);

	//ȣ��Ʈ Ư�� ��������� ��ȯ
	RegisterFileServerResp.Data.opCode = ntohl(RegisterFileServerResp.Data.opCode);
	RegisterFileServerResp.Data.dataLen = ntohl(RegisterFileServerResp.Data.dataLen);
	
	//��� ���� ���� �Ǵ�
	if (RegisterFileServerResp.Data.statusCode == 1)
		printDebugMsg(DC_INFO, errorLevel, "Successfully Connected and Registered to File Server");
	else {
		printDebugMsg(DC_ERROR, errorLevel, "Failled to Register");
		return;
	}

	//change global flag
	fileServerConnectFlag = 1;

	return;
};