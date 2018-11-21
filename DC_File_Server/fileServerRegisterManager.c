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
	@file fileServerRegisterManager.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief FileServer-�߰輭�� ���ó�� ���� �Լ� ����
*/

#include "FileServer.h"

/**
	@fn void procRegisterFileServer(SOCKET hClientSock)
	@brief �߰輭���κ����� ���ϼ��� ��Ͽ�û�� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock �߰輭�� ����� ����
*/
void procRegisterFileServer(SOCKET hClientSock) {
	//��Ŷ �Ҵ� �� �ʱ�ȭ
	sf_RegisterFileServer RegisterFileServer;
	fs_RegisterFileServerResp RegisterFileServerResp;
	memset(&RegisterFileServer, 0, sizeof(sf_RegisterFileServer));
	memset(&RegisterFileServerResp, 0, sizeof(fs_RegisterFileServerResp));

	//��Ŷ ����
	recvData(hClientSock, RegisterFileServer.buf+4, sizeof(sf_RegisterFileServer)-4, 0);
	//ȣ��Ʈ ��������� ����
	RegisterFileServer.Data.opCode = OP_SF_REGISTERFILESERVER;
	RegisterFileServer.Data.dataLen = ntohl(RegisterFileServer.Data.dataLen);

	//��Ŷ ����
	RegisterFileServerResp.Data.opCode = htonl(OP_FS_REGISTERFILESERVERRESP);
	RegisterFileServerResp.Data.dataLen = htonl(0);
	RegisterFileServerResp.Data.statusCode = 1;

	//��� ����
	sendData(hClientSock, RegisterFileServerResp.buf, sizeof(fs_RegisterFileServerResp), 0);

	return;
}