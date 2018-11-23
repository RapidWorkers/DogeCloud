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
	@file FileServer.h
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief FileServer 공통 헤더 파일
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
#include <time.h>

#include "../DC_Common/DC_Common.h"

//동적 라이브러리 로드
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "DC_Common.lib")

//구조체 형식 선언
typedef struct {
	char srvAddr[255];
	unsigned long srvPort;
	char user[255];
	char pass[255];
	char dbase[255];
} MYSQL_SERVER;

typedef struct {
	char authKey[32];
	unsigned int authWaitTime;
	unsigned long userUID;
} DC_AUTHWAIT_LIST;

typedef struct {
	char authKey[32];
	unsigned long userUID;
} DC_FILE_SESSION;

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
	소켓 저장용 구조체 배열
*/
extern SOCKET *hClientSocks;
/**
	@var DC_FILE_SESSION *sessionList
	세션 저장용 구조체 배열
*/
extern DC_FILE_SESSION *sessionList;
/**
	@var DC_AUTHWAIT_LIST *authWaitList
	인증 대기열 저장 구조체 배열
*/
extern DC_AUTHWAIT_LIST *authWaitList;
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
	@var int authWaitCount
	인증 대기 카운트
*/
extern int authWaitCount;
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

//Configuration Reader
/**
	@fn void checkFileConfig()
	@brief 파일서버 설정파일 유무 검사
	@author 멍멍아야옹해봐
*/
void checkFileConfig();
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

//fileServerRegister Manager
/**
	@fn void procRegisterFileServer(SOCKET hClientSock)
	@brief 중계서버로부터의 파일서버 등록요청을 처리
	@author 멍멍아야옹해봐
	@param hClientSock 중계서버 연결된 소켓
*/
void procRegisterFileServer(SOCKET hClientSock);

//user AuthManager
/**
	@fn void procAddUserAuthWaitList(SOCKET hClientSock)
	@brief 중계서버로부터의 유저 인증요청 처리
	@author 멍멍아야옹해봐
	@param hClientSock 중계서버 연결된 소켓
*/
void procAddUserAuthWaitList(SOCKET hClientSock);

/**
	@fn void procFileLogin(SOCKET hClientSock)
	@brief 클라이언트의 파일서버 로그인 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procFileLogin(SOCKET hClientSock);

//garbage collector
/**
	@fn unsigned int WINAPI waitingListGC()
	@brief 대기열 가비지 콜렉터, 인증시간이 초과한 대기열을 삭제함
	@author 멍멍아야옹해봐
*/
unsigned int WINAPI waitingListGC();

//userFileManager
/**
	@fn void procListFile(SOCKET hClientSock)
	@brief 파일 리스트 전송
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procListFile(SOCKET hClientSock);