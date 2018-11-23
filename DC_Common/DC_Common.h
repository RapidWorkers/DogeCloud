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
	@file DC_Common.h
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief DC_Common.dll 로드용 헤더 파일
*/

#pragma once
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include <Windows.h>
#include <stdarg.h>

//동적 라이브러리 로드용
#pragma comment(lib, "ws2_32.lib")

//패킷 구조체 선언된 헤더들
#include "DataManagePacket.h"
#include "SessionPacket.h"
#include "FileServerCommPacket.h"

typedef struct {
	SOCKET *hSocket;
	char clientIP[16];
} DC_SOCK_INFO;

/**
	@fn clearStdinBuffer()
	@brief 표준 입력 버퍼 클리어용 함수
	@author 멍멍아야옹해봐
*/
#define clearStdinBuffer() while(getchar() != '\n')

#ifdef DC_BUILD_DLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
//NOTE: 패킷 구조체의 opCode는 *Packet.h에 정의되어 있음

//프로그램 정보
#define MAJOR_VERSION 0
#define MINOR_VERSION 8
#define VER_STATUS "Pre-Release"

//에러 표기용
#define DC_ERROR 3
#define DC_WARN 2
#define DC_INFO 1
#define DC_DEBUG 0

#ifdef __cplusplus //C++ 환경에서의 호환성 위해
extern "C" {
#endif
	//화면 출력 함수들
	/**
		@fn void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...)
		@brief 디버그 메시지 표시
		@author 멍멍아야옹해봐
		@param targetErrorLevel 오류 수준
		@param currentErrorLevel 설정된 오류 수준
		@param *format 서식 문자열
		@param ... 기타 파라미터
	*/
	DLL void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...);

	/**
		@fn void printProgramInfo()
		@brief DogeCloud 프로그램 정보 표시
		@author 멍멍아야옹해봐
	*/
	DLL void printProgramInfo();

	/**
		@fn void printLicense()
		@brief DogeCloud 라이선스 표시
		@author 멍멍아야옹해봐
	*/
	DLL void printLicense();

	/**
		@fn void updateProgress(int current, int total)
		@brief 프로그레스 바 생성/업데이트
		@author 멍멍아야옹해봐
		@param current 현재 수치
		@param total 전체 수치
	*/
	DLL void updateProgress(int current, int total);

	/**
	@fn void minimizeFileSize(unsigned long size, char *output);
	@brief 파일 사이즈를 최소한으로 표기
	@author 멍멍아야옹해봐
	@param size 파일 사이즈(바이트)
	@param *output 결과값 출력할 곳(사이즈 15 이상)
	*/
	DLL void minimizeFileSize(unsigned long size, char *output);

	//암호화 함수들

	/**
		@fn void SHA256_Text(const char* text, char* buf)
		@brief 문자열의 SHA256 구하기
		@author 멍멍아야옹해봐
		@param *text 구할 문자열
		@param *buf 저장할 공간(32바이트)
	*/
	DLL void SHA256_Text(const char* text, unsigned char* buf);

	/**
		@fn void GenerateSessionKey(char *sessionKey)
		@brief 세션키 생성
		@author 멍멍아야옹해봐
		@param *sessionKey 세션키 저장할 배열(32바이트)
	*/
	DLL void GenerateSessionKey(char *sessionKey);

	/**
		@fn void GenerateCSPRNG(unsigned char *buffer, int numSize)
		@brief 암호학적으로 안전한 난수 생성
		@author 멍멍아야옹해봐
		@param *buffer 저장할 공간
		@param numSize 생성할 크기
	*/
	DLL void GenerateCSPRNG(unsigned char *buffer, int numSize);

	//네트워크 관련 함수
	/**
		@fn bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags)
		@brief 지정할 크기가 전송될 때 까지 전송
		@author 멍멍아야옹해봐
		@param socket 소켓
		@param buffer 패킷 버퍼
		@param sendByte 패킷 사이즈
		@param flags 소켓 플래그
		@return 0 = 실패, 1 = 성공
	*/
	DLL bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags);

	/**
		@fn bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags)
		@brief 지정할 크기가 수신될 때 까지 수신
		@author 멍멍아야옹해봐
		@param socket 소켓
		@param buffer 패킷 버퍼
		@param recvByte 패킷 사이즈
		@param flags 소켓 플래그
		@return 0 = 실패, 1 = 성공
	*/
	DLL bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags);

	//File Related Function

	/**
		@fn void getFileHash(FILE *file, char* result)
		@brief 파일의 SHA256 해쉬 구함
		@author 멍멍아야옹해봐
		@param *file 파일 구조체 포인터
		@param *result 결과를 저장할 공간(32바이트)
	*/
	DLL void getFileHash(FILE *file, unsigned char* result);

	/**
		@fn void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV)
		@brief LEA 암호화 알고리즘으로 파일 암호화
		@author 멍멍아야옹해봐
		@param *infile 입력 파일
		@param *outfile 출력 파일
		@param *encKey 암호화 키(32바이트)
		@param *nonceIV 초기화 난수(16바이트)
	*/
	DLL void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV);

	/**
		@fn void decryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV)
		@brief LEA 암호화 알고리즘으로 파일 복호화
		@author 멍멍아야옹해봐
		@param *infile 입력 파일
		@param *outfile 출력 파일
		@param *encKey 암호화 키(32바이트)
		@param *nonceIV 초기화 난수(16바이트), 암호화 시와 동일 키 사용해야함
	*/
	DLL void decryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV);

#ifdef __cplusplus
}
#endif