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
	@file DogeCloud.h
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief DogeCloud 클라이언트 공통 헤더 파일
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "../DC_Common/DC_Common.h"

//동적 라이브러리 로드
#pragma comment(lib, "DC_Common.lib")
#pragma comment(lib, "ws2_32.lib")

//에러레벨 설정
#define DC_ERRORLEVEL 0

/**
	@fn sendData(hSocket, packetBuffer, packetSize, flag)
	@brief 패킷 전송 처리용 매크로 함수
	@author 멍멍아야옹해봐
	@param hSocket 소켓
	@param packetBuffer 패킷 버퍼
	@param packetSize 패킷 사이즈
	@param flag 소켓 플래그
*/
#define sendData(hSocket, packetBuffer, packetSize, flag) \
if (!sendRaw(hSocket, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");\
	system("pause");\
	exit(1);\
}

/**
	@fn recvData(hSocket, packetBuffer, packetSize, flag)
	@brief 패킷 수신 처리용 매크로 함수
	@author 멍멍아야옹해봐
	@param hSocket 소켓
	@param packetBuffer 패킷 버퍼
	@param packetSize 패킷 사이즈
	@param flag 소켓 플래그
*/
#define recvData(hSocket, packetBuffer, packetSize, flag) \
if (!recvRaw(hSocket, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");\
	system("pause");\
	exit(1);\
}

//전역변수
/**
	@var int loginFlag
	로그인 여부 저장
*/
extern int loginFlag;
/**
	@var char currentUsername[100]
	현재 로그인된 유저 이름 저장
*/
extern char currentUsername[100];
/**
	@var char sessionKey[32]
	중계서버 인증 세션키 저장용 변수
*/
extern char sessionKey[32];

//AuthHelper
/**
	@fn void userLogin(SOCKET hSocket)
	@brief DogeCloud 중계 서버 로그인
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void userLogin(SOCKET hSocket);

/**
	@fn void userRegister(SOCKET hSocket)
	@brief DogeCloud 회원가입
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void userRegister(SOCKET hSocket);

/**
	@fn void userLogout(SOCKET hSocket)
	@brief DogeCloud 중계 서버 로그아웃
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void userLogout(SOCKET hSocket);

//configReader
/**
	@fn void checkDogeConfig()
	@brief DogeCloud 중계 서버 설정파일 유무 확인
	@author 멍멍아야옹해봐
*/
void checkDogeConfig();

/**
	@fn void checkDogeConfig()
	@brief DogeCloud 중계 서버 설정파일 유무 확인
	@author 멍멍아야옹해봐
	@param *RelayServAddr 중계 서버 연결용 주소 구조체 주소
*/
void readRelayServerPath(SOCKADDR_IN *RelayServAddr);

//fileServerConnector
/**
	@fn void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey)
	@brief DogeCloud 파일서버 연결 및 사용자 메뉴 출력
	@author 멍멍아야옹해봐
	@param *fileServerAddr 파일 서버 주소
	@param fileServerPort 파일 서버 포트
	@param *authKey 파일 서버 인증키
*/
void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey);

/**
	@fn void doFileManage(SOCKET hFileSrvSock)
	@brief DogeCloud 파일서버 메뉴 출력
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
*/
void doFileManage(SOCKET hFileSrvSock);

/**
	@fn void showFileList(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud 파일 목록 보여주기
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param *errorFlag 에러 플래그
*/
void showFileList(SOCKET hFileSrvSock, int *errorFlag);

/**
	@fn void moveDir(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud 파일서버 디렉토리 이동
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param *errorFlag 에러 플래그
*/
void moveDir(SOCKET hFileSrvSock, int *errorFlag);

/**
	@fn moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag)
	@brief DogeCloud 파일 목록 페이지 이동
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param type 0= 이전 페이지, 1=다음 페이지
	@param *errorFlag 에러 플래그
*/
void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag);

/**
	@fn void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag)
	@brief DogeCloud 파일 업/다운로드
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param jobType 0= 업로드, 1=다운로드
	@param *errorFlag 에러 플래그
*/
void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag);

//userDataHelper
/**
	@fn void manageFile(SOCKET hSocket)
	@brief DogeCloud 파일서버 처리 함수
	@author 멍멍아야옹해봐
	@param hSocket 중계서버 연결된 소켓
*/
void manageFile(SOCKET hSocket);

/**
	@fn void manageContacts(SOCKET hSocket)
	@brief DogeCloud 연락처 처리 함수
	@author 멍멍아야옹해봐
	@param hSocket 중계서버 연결된 소켓
*/
void manageContacts(SOCKET hSocket);

/**
	@fn void manageMemo(SOCKET hSocket)
	@brief DogeCloud 메모 처리 함수
	@author 멍멍아야옹해봐
	@param hSocket 중계서버 연결된 소켓
*/
void manageMemo(SOCKET hSocket);

//personalDBHelper
/**
	@fn void downloadPersonalDBFile(SOCKET hSocket)
	@brief DogeCloud 개인 DB 파일 다운로드
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void downloadPersonalDBFile(SOCKET hSocket);

/**
	@fn void uploadPersonalDBFile(SOCKET hSocket, char* originalHash)
	@brief DogeCloud 개인 DB 파일 업로드
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
	@param *originalHash 파일 해쉬값
*/
void uploadPersonalDBFile(SOCKET hSocket, char* originalHash);

/**
	@fn void modifyContacts()
	@brief DogeCloud DB에 연락처 추가
	@author 멍멍아야옹해봐
*/
void addContacts();

/**
	@fn void modifyContacts()
	@brief DogeCloud DB에 연락처 수정
	@author 멍멍아야옹해봐
*/
void modifyContacts();

/**
	@fn void deleteContacts()
	@brief DogeCloud DB에 연락처 삭제
	@author 멍멍아야옹해봐
*/
void deleteContacts();

/**
	@fn void addMemo()
	@brief DogeCloud DB에 메모 추가
	@author 멍멍아야옹해봐
*/
void addMemo();

/**
	@fn void modifyMemo()
	@brief DogeCloud DB에 메모 수정
	@author 멍멍아야옹해봐
*/
void modifyMemo();

/**
	@fn void deleteMemo()
	@brief DogeCloud DB에 메모 삭제
	@author 멍멍아야옹해봐
*/
void deleteMemo();

