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
	@author 멍멍아야옹해봐
	@brief FileServer-중계서버 등록처리 관련 함수 모음
*/

#include "FileServer.h"

/**
	@fn void procRegisterFileServer(SOCKET hClientSock)
	@brief 중계서버로부터의 파일서버 등록요청을 처리
	@author 멍멍아야옹해봐
	@param hClientSock 중계서버 연결된 소켓
*/
void procRegisterFileServer(SOCKET hClientSock) {
	//패킷 할당 및 초기화
	sf_RegisterFileServer RegisterFileServer;
	fs_RegisterFileServerResp RegisterFileServerResp;
	memset(&RegisterFileServer, 0, sizeof(sf_RegisterFileServer));
	memset(&RegisterFileServerResp, 0, sizeof(fs_RegisterFileServerResp));

	//패킷 수신
	recvData(hClientSock, RegisterFileServer.buf+4, sizeof(sf_RegisterFileServer)-4, 0);
	//호스트 엔디안으로 변경
	RegisterFileServer.Data.opCode = OP_SF_REGISTERFILESERVER;
	RegisterFileServer.Data.dataLen = ntohl(RegisterFileServer.Data.dataLen);

	//패킷 설정
	RegisterFileServerResp.Data.opCode = htonl(OP_FS_REGISTERFILESERVERRESP);
	RegisterFileServerResp.Data.dataLen = htonl(0);
	RegisterFileServerResp.Data.statusCode = 1;

	//결과 전송
	sendData(hClientSock, RegisterFileServerResp.buf, sizeof(fs_RegisterFileServerResp), 0);

	return;
}