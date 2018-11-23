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
	@author �۸۾ƾ߿��غ�
	@brief DC_Common.dll �ε�� ��� ����
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

//���� ���̺귯�� �ε��
#pragma comment(lib, "ws2_32.lib")

//��Ŷ ����ü ����� �����
#include "DataManagePacket.h"
#include "SessionPacket.h"
#include "FileServerCommPacket.h"

typedef struct {
	SOCKET *hSocket;
	char clientIP[16];
} DC_SOCK_INFO;

/**
	@fn clearStdinBuffer()
	@brief ǥ�� �Է� ���� Ŭ����� �Լ�
	@author �۸۾ƾ߿��غ�
*/
#define clearStdinBuffer() while(getchar() != '\n')

#ifdef DC_BUILD_DLL
#define DLL _declspec(dllexport)
#else
#define DLL _declspec(dllimport)
#endif
//NOTE: ��Ŷ ����ü�� opCode�� *Packet.h�� ���ǵǾ� ����

//���α׷� ����
#define MAJOR_VERSION 0
#define MINOR_VERSION 8
#define VER_STATUS "Pre-Release"

//���� ǥ���
#define DC_ERROR 3
#define DC_WARN 2
#define DC_INFO 1
#define DC_DEBUG 0

#ifdef __cplusplus //C++ ȯ�濡���� ȣȯ�� ����
extern "C" {
#endif
	//ȭ�� ��� �Լ���
	/**
		@fn void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...)
		@brief ����� �޽��� ǥ��
		@author �۸۾ƾ߿��غ�
		@param targetErrorLevel ���� ����
		@param currentErrorLevel ������ ���� ����
		@param *format ���� ���ڿ�
		@param ... ��Ÿ �Ķ����
	*/
	DLL void printDebugMsg(int targetErrorLevel, int currentErrorLevel, const char* format, ...);

	/**
		@fn void printProgramInfo()
		@brief DogeCloud ���α׷� ���� ǥ��
		@author �۸۾ƾ߿��غ�
	*/
	DLL void printProgramInfo();

	/**
		@fn void printLicense()
		@brief DogeCloud ���̼��� ǥ��
		@author �۸۾ƾ߿��غ�
	*/
	DLL void printLicense();

	/**
		@fn void updateProgress(int current, int total)
		@brief ���α׷��� �� ����/������Ʈ
		@author �۸۾ƾ߿��غ�
		@param current ���� ��ġ
		@param total ��ü ��ġ
	*/
	DLL void updateProgress(int current, int total);

	/**
	@fn void minimizeFileSize(unsigned long size, char *output);
	@brief ���� ����� �ּ������� ǥ��
	@author �۸۾ƾ߿��غ�
	@param size ���� ������(����Ʈ)
	@param *output ����� ����� ��(������ 15 �̻�)
	*/
	DLL void minimizeFileSize(unsigned long size, char *output);

	//��ȣȭ �Լ���

	/**
		@fn void SHA256_Text(const char* text, char* buf)
		@brief ���ڿ��� SHA256 ���ϱ�
		@author �۸۾ƾ߿��غ�
		@param *text ���� ���ڿ�
		@param *buf ������ ����(32����Ʈ)
	*/
	DLL void SHA256_Text(const char* text, unsigned char* buf);

	/**
		@fn void GenerateSessionKey(char *sessionKey)
		@brief ����Ű ����
		@author �۸۾ƾ߿��غ�
		@param *sessionKey ����Ű ������ �迭(32����Ʈ)
	*/
	DLL void GenerateSessionKey(char *sessionKey);

	/**
		@fn void GenerateCSPRNG(unsigned char *buffer, int numSize)
		@brief ��ȣ�������� ������ ���� ����
		@author �۸۾ƾ߿��غ�
		@param *buffer ������ ����
		@param numSize ������ ũ��
	*/
	DLL void GenerateCSPRNG(unsigned char *buffer, int numSize);

	//��Ʈ��ũ ���� �Լ�
	/**
		@fn bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags)
		@brief ������ ũ�Ⱑ ���۵� �� ���� ����
		@author �۸۾ƾ߿��غ�
		@param socket ����
		@param buffer ��Ŷ ����
		@param sendByte ��Ŷ ������
		@param flags ���� �÷���
		@return 0 = ����, 1 = ����
	*/
	DLL bool sendRaw(SOCKET socket, char* buffer, int sendByte, int flags);

	/**
		@fn bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags)
		@brief ������ ũ�Ⱑ ���ŵ� �� ���� ����
		@author �۸۾ƾ߿��غ�
		@param socket ����
		@param buffer ��Ŷ ����
		@param recvByte ��Ŷ ������
		@param flags ���� �÷���
		@return 0 = ����, 1 = ����
	*/
	DLL bool recvRaw(SOCKET socket, char* buffer, int recvByte, int flags);

	//File Related Function

	/**
		@fn void getFileHash(FILE *file, char* result)
		@brief ������ SHA256 �ؽ� ����
		@author �۸۾ƾ߿��غ�
		@param *file ���� ����ü ������
		@param *result ����� ������ ����(32����Ʈ)
	*/
	DLL void getFileHash(FILE *file, unsigned char* result);

	/**
		@fn void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV)
		@brief LEA ��ȣȭ �˰������� ���� ��ȣȭ
		@author �۸۾ƾ߿��غ�
		@param *infile �Է� ����
		@param *outfile ��� ����
		@param *encKey ��ȣȭ Ű(32����Ʈ)
		@param *nonceIV �ʱ�ȭ ����(16����Ʈ)
	*/
	DLL void encryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV);

	/**
		@fn void decryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV)
		@brief LEA ��ȣȭ �˰������� ���� ��ȣȭ
		@author �۸۾ƾ߿��غ�
		@param *infile �Է� ����
		@param *outfile ��� ����
		@param *encKey ��ȣȭ Ű(32����Ʈ)
		@param *nonceIV �ʱ�ȭ ����(16����Ʈ), ��ȣȭ �ÿ� ���� Ű ����ؾ���
	*/
	DLL void decryptFileLEA(FILE *infile, FILE *outfile, char* encKey, char* nonceIV);

#ifdef __cplusplus
}
#endif