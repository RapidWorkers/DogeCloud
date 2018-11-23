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
	@author 멍멍아야옹해봐
	@brief 중계서버 소켓 핸들러
*/

#include "RelayServer.h"

/**
	@fn unsigned int WINAPI clientHandler(void* clientInfo)
	@brief 클라이언트 핸들러
	@author 멍멍아야옹해봐
	@param clientInfo (DC_SOCK_INFO)를 void*로 캐스팅한 포인터
*/
unsigned int WINAPI clientHandler(void* clientInfo) {
	//변수의 사용을 위해 캐스팅
	SOCKET hClientSock = *(((DC_SOCK_INFO*)clientInfo)->hSocket);
	char clientIP[16];
	memcpy_s(clientIP, 16, ((DC_SOCK_INFO*)clientInfo)->clientIP, 16);

	unsigned long opCode = 0;

	while (recvRaw(hClientSock, (char*)&opCode, 4, 0)) {//연결이 끊길때까지 옵코드를 클라이언트에서 받아옴
		opCode = ntohl(opCode);//호스트 엔디안으로 변환
		packetHandler(hClientSock, clientIP, opCode);//패킷 핸들러로 넘김
	}

	printDebugMsg(1, errorLevel, "Connection Closed");

	//연결이 종료되면 설정되었던 전역변수를 삭제
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			while (i < clientCount - 1) {
				hClientSocks[i] = hClientSocks[i + 1];
				memcpy(&sessionList[i], &sessionList[i + 1], sizeof(DC_SESSION));
				i++;
			}
			//변수 0으로 설정 => 완전 삭제
			memset(&sessionList[i], 0, sizeof(DC_SESSION));
			memset(&hClientSocks[i], 0, sizeof(SOCKET));
			break;
		}
	}
	clientCount--;//클라이언트 카운트 감소
	printDebugMsg(DC_INFO, errorLevel, "Connection Limit: %d / %d", clientCount, maxConnection);
	ReleaseMutex(hMutex);

	closesocket(hClientSock);//소켓 종료
	return 0;//쓰레드 종료
}

/**
	@fn void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode)
	@brief 패킷 핸들러
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트가 연결된 소켓
	@param *clientIP 클라이언트 IP
	@param opCode 패킷 opCode
*/
void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode) {
	if (hClientSock == INVALID_SOCKET) return;//유효하지 않은 소켓이면 그냥 끝냄

	printDebugMsg(DC_DEBUG, errorLevel, "Got OpCode: %d", opCode);
	switch (opCode) {//opCode에 따라 선택
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