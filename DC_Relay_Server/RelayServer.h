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
	@file RelayServer.h
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief RelayServer 공통 헤더 파일
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <process.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <mysql.h>

#include "../DC_Common/DC_Common.h"

//dll 로드
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DC_Common.lib")

typedef struct {
	char srvAddr[255];
	unsigned long srvPort;
	char user[255];
	char pass[255];
	char dbase[255];
} MYSQL_SERVER;

typedef struct {
	unsigned long userUID;
	char currentStatus;
	char sessionKey[32];
} DC_SESSION;

/**
	@fn sendData(hClientSock, packetBuffer, packetSize, flag)
	@brief 패킷 전송 처리용 매크로 함수
	@author 멍멍아야옹해봐
	@param hClientSock 소켓
	@param packetBuffer 패킷 버퍼
	@param packetSize 패킷 사이즈
	@param flag 소켓 플래그
*/
#define sendData(hClientSock, packetBuffer, packetSize, flag) \
if (!sendRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, errorLevel, "Connection Error: %d", WSAGetLastError());\
	return;\
}

/**
	@fn recvData(hClientSock, packetBuffer, packetSize, flag)
	@brief 패킷 전송 처리용 매크로 함수
	@author 멍멍아야옹해봐
	@param hClientSock 소켓
	@param packetBuffer 패킷 버퍼
	@param packetSize 패킷 사이즈
	@param flag 소켓 플래그
*/
#define recvData(hClientSock, packetBuffer, packetSize, flag) \
if (!recvRaw(hClientSock, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, errorLevel, "Connection Error: %d", WSAGetLastError());\
	return;\
}


//전역변수 선언
/**
	@var HANDLE hMutex
	전역변수 보호용 뮤텍스
*/
extern HANDLE hMutex;
/**
@var SOCKET *hClientSocks
소켓 저장용 구조체 배열 포인터(동적할당으로 생성됨)
*/
extern SOCKET *hClientSocks;
/**
@var DC_SESSION *sessionList
세션 저장용 구조체 배열 포인터(동적할당으로 생성됨)
*/
extern DC_SESSION *sessionList;

/**
@var int maxConnection
최대 접속수
*/
extern int maxConnection;

/**
	@var int clientCount
	접속 카운트
*/
extern int clientCount;

/**
	@var MYSQL_SERVER serverInfo
	MySQL 서버 주소 담는 구조체
*/
extern MYSQL_SERVER serverInfo;
/**
	@var MYSQL sqlHandle
	MySQL 핸들
*/
extern MYSQL sqlHandle;

/**
	@var SOCKADDR_IN fileSrvAddr
	파일서버 주소 저장용 구조체
*/
extern SOCKADDR_IN fileSrvAddr;
/**
	@var SOCKET hFileSrvSock
	파일서버 소켓
*/
extern SOCKET hFileSrvSock;
/**
	@var int fileServerConnectFlag
	파일서버 연결 플래그
*/
extern int fileServerConnectFlag;

/**
	@var int errorLevel
	디버그 표시용 에러레벨
*/
extern int errorLevel;

//socket Handler
/**
	@fn unsigned int WINAPI clientHandler(void* clientInfo)
	@brief 클라이언트 핸들러
	@author 멍멍아야옹해봐
	@param clientInfo (DC_SOCK_INFO)를 void*로 캐스팅한 포인터
*/
unsigned int WINAPI clientHandler(void* clientInfo);

/**
	@fn void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode)
	@brief 패킷 핸들러
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트가 연결된 소켓
	@param *clientIP 클라이언트 IP
	@param opCode 패킷 opCode
*/
void packetHandler(SOCKET hClientSock, const char *clientIP, unsigned long opCode);

//Mariadb Connector

/**
	@fn void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo)
	@brief SQL 서버 접속
	@author 멍멍아야옹해봐
	@param *sqlHandle MySQL Handle 포인터
	@param serverInfo MySQL 서버 정보 구조체
*/
void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo);

/**
	@fn void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind)
	@brief SQL Prepared Statement 준비 및 쿼리 실행
	@author 멍멍아야옹해봐
	@param *sqlHandle MySQL Handle 포인터
	@param *stmt MySQL stmt 포인터
	@param *query 실행할 쿼리문
	@param *query_bind 쿼리에 바인드할 것의 포인터
	@param *result_bind 결과물에 바인드할 것의 포인터
*/
void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind);

//Session Proccesors

/**
	@fn void procLoginStart(SOCKET hClientSock)
	@brief 로그인 처리 시작
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procLoginStart(SOCKET hClientSock);

/**
	@fn void doLogin(SOCKET hClientSock)
	@brief 실제 로그인 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void doLogin(SOCKET hClientSock);

/**
	@fn void procLogout(SOCKET hClientSock)
	@brief 로그아웃 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procLogout(SOCKET hClientSock);

/**
	@fn void procRegisterStart(SOCKET hClientSock)
	@brief 회원가입 처리 시작
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procRegisterStart(SOCKET hClientSock);

/**
	@fn void doRegister(SOCKET hClientSock)
	@brief 실제 회원가입 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void doRegister(SOCKET hClientSock);

//Configuration Reader
/**
	@fn void checkRelayConfig()
	@brief 중계서버 설정파일 유무 검사
	@author 멍멍아야옹해봐
*/
void checkRelayConfig();

/**
	@fn void setErrorLevel()
	@brief 에러레벨 설정
	@author 멍멍아야옹해봐
*/
void setErrorLevel();

/**
@fn int readMaxConn()
@brief 최대 접속수 반환
@author 멍멍아야옹해봐
@return 최대 접속수
*/
int readMaxConn();

/**
@fn void readBindInfo(SOCKADDR_IN *servAddr)
@brief 서버 IP, 포트 설정 읽어옴
@author 멍멍아야옹해봐
@param *servAddr 서버 주소 정보 저장 구조체 포인터
*/
void readBindInfo(SOCKADDR_IN *servAddr);

/**
	@fn void readMySQLConfig(MYSQL_SERVER *serverInfo)
	@brief MySQL 서버 설정 읽어옴
	@author 멍멍아야옹해봐
	@param *serverInfo MySQL 서버 정보 구조체 포인터
*/
void readMySQLConfig(MYSQL_SERVER *serverInfo);

/**
	@fn void readFileServerPath(SOCKADDR_IN *FileServAddr)
	@brief DogeCloud 파일 서버 경로 읽어오기
	@author 멍멍아야옹해봐
	@param *FileServAddr 중계 서버 연결용 주소 구조체 주소
*/
void readFileServerPath(SOCKADDR_IN *FileServAddr);

//fileServer Register
/**
	@fn void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *FileServAddr)
	@brief 파일서버 연결
	@author 멍멍아야옹해봐
	@param *hFileSrvSock 파일서버 연결용 소켓
	@param *FileServAddr 중계 서버 연결용 주소 구조체 주소
*/
void initFSConnection(SOCKET *hFileSrvSock, SOCKADDR_IN *FileServAddr);

//user to FileServer bridge
/**
	@fn void procFileServerConnReq(SOCKET hClientSock)
	@brief 사용자의 파일서버 연결요청 처리
	@author 멍멍아야옹해봐
	@param hClientSock 사용자 연결된 소켓
*/
void procFileServerConnReq(SOCKET hClientSock);

/**
	@fn void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag)
	@brief 사용자를 파일서버에 인증시킴
	@author 멍멍아야옹해봐
	@param *authKey 인증키 문자열
	@param userUID 유저 UID
	@param resultFlag 결과 플래그
*/
void authFSUser(unsigned char* authKey, unsigned long userUID, int *resultFlag);

//personalDBHandler
/**
	@fn void procDownloadPersonalDBFile(SOCKET hClientSock)
	@brief 사용자의 개인 DB 다운로드 요청 처리
	@author 멍멍아야옹해봐
	@param hClientSock 사용자 연결된 소켓
*/
void procDownloadPersonalDBFile(SOCKET hClientSock);
/**
	@fn void procUploadPersonalDBFile(SOCKET hClientSock)
	@brief 사용자의 개인 DB 업로드 요청 처리
	@author 멍멍아야옹해봐
	@param hClientSock 사용자 연결된 소켓
*/
void procUploadPersonalDBFile(SOCKET hClientSock);