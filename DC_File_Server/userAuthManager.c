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
	@author 멍멍아야옹해봐
	@brief 사용자 인증 관련 함수 모음
*/

#include "FileServer.h"

/**
	@fn void procAddUserAuthWaitList(SOCKET hClientSock)
	@brief 중계서버로부터의 유저 인증요청 처리
	@author 멍멍아야옹해봐
	@param hClientSock 중계서버 연결된 소켓
*/
void procAddUserAuthWaitList(SOCKET hClientSock) {
	//패킷을 위한 공간 할당 및 초기화
	sf_AuthUser AuthUser;
	fs_AuthUserResp AuthUserResp;
	memset(&AuthUser, 0, sizeof(sf_AuthUser));
	memset(&AuthUserResp, 0, sizeof(fs_AuthUserResp));

	//유저로부터 남은 데이터 가져옴
	recvData(hClientSock, AuthUser.buf + 4, sizeof(AuthUser) - 4, 0);

	//호스트 엔디안으로 변경
	AuthUser.Data.opCode = ntohl(AuthUser.Data.opCode);
	AuthUser.Data.dataLen = ntohl(AuthUser.Data.dataLen);
	AuthUser.Data.userUID = ntohl(AuthUser.Data.userUID);

	//응답 패킷 설정
	AuthUserResp.Data.opCode = htonl(OP_FS_AUTHUSERRESP);
	AuthUserResp.Data.dataLen = htonl(sizeof(AuthUserResp) - 8);

	if (authWaitCount+clientCount >= maxConnection) {//만약 인증 대기 + 접속 수가 최대 접속수를 초과하면
		AuthUserResp.Data.statusCode = 0;//접속 거부
		printDebugMsg(DC_WARN, errorLevel, "WARN: Auth Waiting List and Server Connection is FULL.");
	}
	else {
		//전역변수에 변수 세팅 => 인증 대기열에 추가
		memcpy(authWaitList[authWaitCount].authKey, AuthUser.Data.UserFileServerAuthKey, 32);//add authkey to waiting list
		authWaitList[authWaitCount].authWaitTime = (unsigned int)time(NULL); //save auth start time => prevent infinite auth wait
		authWaitList[authWaitCount++].userUID = AuthUser.Data.userUID;
		AuthUserResp.Data.statusCode = 1;//접속 허용
	}

	//응답 전송
	sendData(hClientSock, AuthUserResp.buf, sizeof(fs_AuthUserResp), 0);

	return;
}

/**
	@fn void procFileLogin(SOCKET hClientSock)
	@brief 클라이언트의 파일서버 로그인 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procFileLogin(SOCKET hClientSock) {
	//패킷 할당 및 0으로 초기화
	cf_LoginFile LoginFile;
	fc_LoginFileResp LoginFileResp;
	memset(&LoginFile, 0, sizeof(cf_LoginFile));
	memset(&LoginFileResp, 0, sizeof(fc_LoginFileResp));

	//남은 응답 다 받아옴
	recvData(hClientSock, LoginFile.buf + 4, sizeof(cf_LoginFile) - 4, 0);
	
	//호스트 엔디안으로 변환
	LoginFile.Data.opCode = ntohl(LoginFile.Data.opCode);
	LoginFile.Data.dataLen = ntohl(LoginFile.Data.dataLen);

	//패킷 데이터 설정
	LoginFileResp.Data.statusCode = 0;//일단 0으로

	int userUID = 0;

	//인증 대기열에서 인증 요청 받은 키 검색
	WaitForSingleObject(hMutex, INFINITE);
	for (int i = 0; i < authWaitCount; i++) {
		if (!memcmp(LoginFile.Data.UserFileServerAuthKey, authWaitList[i].authKey, 32)) {//일치하는 키 값이 있을 경우
			for (int j = 0; j < clientCount; j++) {
				if (hClientSock == hClientSocks[j]) {
					memcpy(sessionList[j].authKey, LoginFile.Data.UserFileServerAuthKey, 32); //copy generated sessionKey
					sessionList[j].userUID = authWaitList[i].userUID;
					authWaitList[i].authWaitTime = 0; //GC가 인증이 완료된 목록을 자동으로 제거하도록
					LoginFileResp.Data.statusCode = 1;//인증 성공
					userUID = sessionList[j].userUID;
					break;
				}
			}
			break;
		}
	}
	ReleaseMutex(hMutex);

	//패킷 설정
	LoginFileResp.Data.opCode = htonl(OP_FC_LOGINFILERESP);
	LoginFileResp.Data.dataLen = htonl(sizeof(fc_LoginFileResp) - 8);

	//응답 전송
	sendData(hClientSock, LoginFileResp.buf, sizeof(fc_LoginFileResp), 0);

	printDebugMsg(DC_INFO, errorLevel, "Auth Done.");

	return;
}