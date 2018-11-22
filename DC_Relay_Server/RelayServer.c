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
	@file RelayServer.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief RelayServer 메인 파일
*/

#include "RelayServer.h"

/**
	@var HANDLE hMutex
	전역변수 보호용 뮤텍스
*/
HANDLE hMutex;

//create Client Sock array
/**
	@var SOCKET hClientSocks[MAX_CON]
	소켓 저장용 구조체 배열
*/
SOCKET hClientSocks[MAX_CON];
/**
	@var DC_SESSION sessionList[MAX_CON]
	세션 저장용 구조체 배열
*/
DC_SESSION sessionList[MAX_CON];
/**
	@var int clientCount
	접속 카운트
*/
int clientCount;

/**
	@var SOCKET hFileSrvSock
	파일서버 소켓
*/
SOCKET hFileSrvSock;
/**
	@var SOCKADDR_IN fileSrvAddr
	파일서버 주소 저장용 구조체
*/
SOCKADDR_IN fileSrvAddr;
/**
	@var int fileServerConnectFlag
	파일서버 연결 플래그
*/
int fileServerConnectFlag;

/**
	@var MYSQL_SERVER serverInfo
	MySQL 서버 주소 담는 구조체
*/
MYSQL_SERVER serverInfo;
/**
	@var MYSQL sqlHandle
	MySQL 핸들
*/
MYSQL sqlHandle;

/**
	@var int errorLevel
	디버그 표시용 에러레벨
*/
int errorLevel;

/**
	@fn int main()
	@brief RelayServer 진입점
	@author 멍멍아야옹해봐
*/
int main()
{
	/** @brief wsa 라이브러리용 구조체 */
	WSADATA wsaData;
	/** @brief 서버용 소켓, 클라이언트 접속 처리용 소켓 */
	SOCKET hServSock, hClientSock;
	/** @brief 서버 주소, 클라이언트 주소 담는 구조체 */
	SOCKADDR_IN servAddr, clientAddr;

	/** @brief 멀티쓰레드 핸들 */
	HANDLE hThread = NULL;
	//unsigned int threadID;

	//뮤텍스 생성
	hMutex = CreateMutex(NULL, FALSE, NULL);
	//DWORD dwErrorCode = 0;

	//에러레벨 읽어오고 설정
	setErrorLevel();

	printProgramInfo();

	//데이터베이스 설정 읽고 접속
	readMySQLConfig(&serverInfo);
	sqlInit(&sqlHandle, serverInfo);

	//연락처, 메모 db 저장용 폴더 생성
	if (!CreateDirectory("./infodb", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {//이미 폴더가 존재할 때의 경우를 제외한 모든 에러는 프로그램 정지
			printDebugMsg(DC_ERROR, errorLevel, "Create Directory Fail");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}
	};

	//소켓 초기화 시작
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {//WSA Startup
		printDebugMsg(DC_ERROR, errorLevel, "WSAStartup Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	hServSock = socket(PF_INET, SOCK_STREAM, 0);//서버 소켓 초기화
	if (hServSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, errorLevel, "Invalid Server Socket");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	//서버 주소 설정
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(BIND_PORT);

	if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr))) {//바인드
		printDebugMsg(DC_ERROR, errorLevel, "Bind Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	//파일서버 경로 읽어오기
	readFileServerPath(&fileSrvAddr);
	for (int i = 0; i < 5; i++) {//파일서버 5번 접속 시도
		printDebugMsg(DC_INFO, errorLevel, "Trying to Connect File Server %d / 5", i+1);
		initFSConnection(&hFileSrvSock, &fileSrvAddr);
		if (fileServerConnectFlag) break;
		Sleep(1000);//다음 접속까지 1초 대기
	}

	if (!fileServerConnectFlag) {//파일서버 연결에 실패하였을 경우
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: File Server Not Online!!");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	if (listen(hServSock, 5) == SOCKET_ERROR) {//서버 리스닝 시작
		printDebugMsg(DC_ERROR, errorLevel, "Listen Fail");
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);
	}

	printDebugMsg(DC_INFO, errorLevel, "Server Started");
	printDebugMsg(DC_INFO, errorLevel, "Server Listening at %s:%d", "NOT IMPLEMENTED", 0);

	//클라이언트 접속 대기
	while (1) {
		int szClntAddr = sizeof(clientAddr);
		hClientSock = accept(hServSock, (SOCKADDR*)&clientAddr, &szClntAddr);

		if (hClientSock == INVALID_SOCKET) {//클라이언트 접속 실패시
			printDebugMsg(DC_ERROR, errorLevel, "Client Accept Fail");
			break;
		}

		if (clientCount >= MAX_CON) {//접속 제한 도달시
			printDebugMsg(DC_WARN, errorLevel, "MAX CONNECTION REACHED, CLOSE CONNECTION!!");
			closesocket(hClientSock);
			continue;
		}

		WaitForSingleObject(hMutex, INFINITE);
		hClientSocks[clientCount++] = hClientSock;
		/** @brief 클라이언트 정보 담은 구조체 */
		DC_SOCK_INFO clientInfo;
		clientInfo.hSocket = &hClientSock;
		inet_ntop(AF_INET, &clientAddr.sin_addr, clientInfo.clientIP, 16);//클라이언트 ip 문자열로 변환
		printDebugMsg(DC_INFO, errorLevel, "Connection Limit: %d / %d", clientCount, MAX_CON);
		ReleaseMutex(hMutex);
		hThread = (HANDLE)_beginthreadex(NULL, 0, clientHandler, (void*)&clientInfo, 0, NULL);//쓰레드 생성하여 넘김
		printDebugMsg(DC_INFO, errorLevel, "Client Connected: %s", clientInfo.clientIP);

	}

	closesocket(hServSock);//서버 종료시 소켓 종료
	WSACleanup();//라이브러리 해제
	printDebugMsg(1, errorLevel, "Server Terminated");
	system("pause");
	return 0;
}