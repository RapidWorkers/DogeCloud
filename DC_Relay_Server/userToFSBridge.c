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
	@author 멍멍아야옹해봐
	@brief 사용자-파일서버 연결 함수
*/

#include "RelayServer.h"

/**
	@fn void procFileServerConnReq(SOCKET hClientSock)
	@brief 사용자의 파일서버 연결요청 처리
	@author 멍멍아야옹해봐
	@param hClientSock 사용자 연결된 소켓
*/
void procFileServerConnReq(SOCKET hClientSock) {

	//if (!checkStatus(hFileSrvSock)) {
	//	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Lost File Server Connection");
	//	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
	//	system("pause");
	//	exit(1);
	//}

	//패킷 설정 및 초기화
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	//남아있는 데이터 받아옴
	recvData(hClientSock, FileSrvConReq.buf, sizeof(cs_FileSrvConReq) - 4, 0);

	//호스트 특정 엔디안으로 변환
	FileSrvConReq.Data.opCode = OP_CS_FILESRVCONNREQ;
	FileSrvConReq.Data.dataLen = ntohl(FileSrvConReq.Data.dataLen);

	//패킷 설정
	FileSrvConResp.Data.opCode = htonl(OP_SC_FILESRVCONNRESP);
	FileSrvConResp.Data.dataLen = htonl(sizeof(sc_FileSrvConResp) - 8);
	
	unsigned char authKey[32];
	unsigned long userUID;

	//임의의 인증키 생성
	GenerateSessionKey(authKey);

	//전역변수에서 UID 가져옴
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			userUID = sessionList[i].userUID;
			break;
		}
	}
	ReleaseMutex(hMutex);

	int authResult = 0;

	//파일서버 인증 처리
	authFSUser(authKey, userUID, &authResult);
	if(authResult){//인증 성공시
		memcpy(FileSrvConResp.Data.authKey, authKey, 32);//패킷에 인증키 복사
		inet_ntop(AF_INET, &fileSrvAddr.sin_addr, FileSrvConResp.Data.fileSrvAddr, 16);
		FileSrvConResp.Data.fileSrvPort = ntohs(fileSrvAddr.sin_port);
		FileSrvConResp.Data.statusCode = 1;
	} else {//인증 실패시
		FileSrvConResp.Data.statusCode = 0;
	}

	//패킷 전송
	sendData(hClientSock, FileSrvConResp.buf, sizeof(FileSrvConResp), 0);

	return;
}

/**
	@fn void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag)
	@brief 사용자를 파일서버에 인증시킴
	@author 멍멍아야옹해봐
	@param *authKey 인증키 문자열
	@param userUID 유저 UID
	@param resultFlag 결과 플래그
*/
void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag) {
	//패킷 설정 및 초기화
	sf_AuthUser AuthUser;
	fs_AuthUserResp AuthUserResp;
	memset(&AuthUser, 0, sizeof(sf_AuthUser));
	memset(&AuthUserResp, 0, sizeof(fs_AuthUserResp));

	//패킷 데이터 설정
	AuthUser.Data.opCode = htonl(OP_SF_AUTHUSER);
	AuthUser.Data.dataLen = htonl(sizeof(AuthUser) - 8);
	AuthUser.Data.userUID = htonl(userUID);
	memcpy(AuthUser.Data.UserFileServerAuthKey, authKey, 32);

	//패킷 전송
	sendData(hFileSrvSock, AuthUser.buf, sizeof(sf_AuthUser), 0);
	//응답 받음
	recvData(hFileSrvSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	//호스트 특정 엔디안으로 변환
	AuthUserResp.Data.opCode = ntohl(AuthUserResp.Data.opCode);
	AuthUserResp.Data.dataLen = ntohl(AuthUserResp.Data.dataLen);
	
	//성공유무 반환
	if (AuthUserResp.Data.statusCode == 1)
		*resultFlag = 1;
	else
		*resultFlag = 0;

	return;
}