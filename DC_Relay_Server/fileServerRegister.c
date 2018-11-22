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
	@author 멍멍아야옹해봐
	@brief 파일서버 등록처리 함수 모음
*/

#include "RelayServer.h"

/**
	@fn void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *FileServAddr)
	@brief 파일서버 연결
	@author 멍멍아야옹해봐
	@param *hFileSrvSock 파일서버 연결용 소켓
	@param *FileServAddr 중계 서버 연결용 주소 구조체 주소
*/
void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *fileSrvAddr) {
	//패킷 선언 및 초기화
	sf_RegisterFileServer RegisterFileServer;
	fs_RegisterFileServerResp RegisterFileServerResp;
	memset(&RegisterFileServer, 0, sizeof(sf_RegisterFileServer));
	memset(&RegisterFileServerResp, 0, sizeof(fs_RegisterFileServerResp));

	//소켓 생성
	*hFileSrvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (*hFileSrvSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, errorLevel, "Invalid Socket");
		return;
	}

	//실제 접속
	int err = (connect(*hFileSrvSock, (SOCKADDR*)fileSrvAddr, sizeof(*fileSrvAddr)) == SOCKET_ERROR);
	if (err) //생성된 소켓으로 서버에 연결
	{
		printDebugMsg(DC_ERROR, errorLevel, "FS Connection Failled: %d", WSAGetLastError());
		return;
	}

	//패킷 설정
	RegisterFileServer.Data.opCode = htonl(OP_SF_REGISTERFILESERVER);
	RegisterFileServer.Data.dataLen = htonl(0);

	//등록 패킷 전송
	sendData(*hFileSrvSock, RegisterFileServer.buf, sizeof(sf_RegisterFileServer), 0);
	//응답 수신
	recvData(*hFileSrvSock, RegisterFileServerResp.buf, sizeof(fs_RegisterFileServerResp), 0);

	//호스트 특정 엔디안으로 변환
	RegisterFileServerResp.Data.opCode = ntohl(RegisterFileServerResp.Data.opCode);
	RegisterFileServerResp.Data.dataLen = ntohl(RegisterFileServerResp.Data.dataLen);
	
	//등록 성공 유무 판단
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