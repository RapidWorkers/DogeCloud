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
	@author �۸۾ƾ߿��غ�
	@brief DogeCloud Ŭ���̾�Ʈ ���� ��� ����
*/

#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "../DC_Common/DC_Common.h"

//���� ���̺귯�� �ε�
#pragma comment(lib, "DC_Common.lib")
#pragma comment(lib, "ws2_32.lib")

//�������� ����
#define DC_ERRORLEVEL 0

/**
	@fn sendData(hSocket, packetBuffer, packetSize, flag)
	@brief ��Ŷ ���� ó���� ��ũ�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket ����
	@param packetBuffer ��Ŷ ����
	@param packetSize ��Ŷ ������
	@param flag ���� �÷���
*/
#define sendData(hSocket, packetBuffer, packetSize, flag) \
if (!sendRaw(hSocket, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ���ῡ ������ �߻��߽��ϴ�: %d", WSAGetLastError());\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");\
	system("pause");\
	exit(1);\
}

/**
	@fn recvData(hSocket, packetBuffer, packetSize, flag)
	@brief ��Ŷ ���� ó���� ��ũ�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket ����
	@param packetBuffer ��Ŷ ����
	@param packetSize ��Ŷ ������
	@param flag ���� �÷���
*/
#define recvData(hSocket, packetBuffer, packetSize, flag) \
if (!recvRaw(hSocket, packetBuffer, packetSize, flag)) {\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ���ῡ ������ �߻��߽��ϴ�: %d", WSAGetLastError());\
	printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");\
	system("pause");\
	exit(1);\
}

//��������
/**
	@var int loginFlag
	�α��� ���� ����
*/
extern int loginFlag;
/**
	@var char currentUsername[100]
	���� �α��ε� ���� �̸� ����
*/
extern char currentUsername[100];
/**
	@var char sessionKey[32]
	�߰輭�� ���� ����Ű ����� ����
*/
extern char sessionKey[32];

//AuthHelper
/**
	@fn void userLogin(SOCKET hSocket)
	@brief DogeCloud �߰� ���� �α���
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void userLogin(SOCKET hSocket);

/**
	@fn void userRegister(SOCKET hSocket)
	@brief DogeCloud ȸ������
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void userRegister(SOCKET hSocket);

/**
	@fn void userLogout(SOCKET hSocket)
	@brief DogeCloud �߰� ���� �α׾ƿ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void userLogout(SOCKET hSocket);

//configReader
/**
	@fn void checkDogeConfig()
	@brief DogeCloud �߰� ���� �������� ���� Ȯ��
	@author �۸۾ƾ߿��غ�
*/
void checkDogeConfig();

/**
	@fn void checkDogeConfig()
	@brief DogeCloud �߰� ���� �������� ���� Ȯ��
	@author �۸۾ƾ߿��غ�
	@param *RelayServAddr �߰� ���� ����� �ּ� ����ü �ּ�
*/
void readRelayServerPath(SOCKADDR_IN *RelayServAddr);

//fileServerConnector
/**
	@fn void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey)
	@brief DogeCloud ���ϼ��� ���� �� ����� �޴� ���
	@author �۸۾ƾ߿��غ�
	@param *fileServerAddr ���� ���� �ּ�
	@param fileServerPort ���� ���� ��Ʈ
	@param *authKey ���� ���� ����Ű
*/
void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey);

/**
	@fn void doFileManage(SOCKET hFileSrvSock)
	@brief DogeCloud ���ϼ��� �޴� ���
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
*/
void doFileManage(SOCKET hFileSrvSock);

/**
	@fn void showFileList(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud ���� ��� �����ֱ�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param *errorFlag ���� �÷���
*/
void showFileList(SOCKET hFileSrvSock, int *errorFlag);

/**
	@fn void moveDir(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud ���ϼ��� ���丮 �̵�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param *errorFlag ���� �÷���
*/
void moveDir(SOCKET hFileSrvSock, int *errorFlag);

/**
	@fn moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag)
	@brief DogeCloud ���� ��� ������ �̵�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param type 0= ���� ������, 1=���� ������
	@param *errorFlag ���� �÷���
*/
void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag);

/**
	@fn void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag)
	@brief DogeCloud ���� ��/�ٿ�ε�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param jobType 0= ���ε�, 1=�ٿ�ε�
	@param *errorFlag ���� �÷���
*/
void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag);

//userDataHelper
/**
	@fn void manageFile(SOCKET hSocket)
	@brief DogeCloud ���ϼ��� ó�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰輭�� ����� ����
*/
void manageFile(SOCKET hSocket);

/**
	@fn void manageContacts(SOCKET hSocket)
	@brief DogeCloud ����ó ó�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰輭�� ����� ����
*/
void manageContacts(SOCKET hSocket);

/**
	@fn void manageMemo(SOCKET hSocket)
	@brief DogeCloud �޸� ó�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰輭�� ����� ����
*/
void manageMemo(SOCKET hSocket);

//personalDBHelper
/**
	@fn void downloadPersonalDBFile(SOCKET hSocket)
	@brief DogeCloud ���� DB ���� �ٿ�ε�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void downloadPersonalDBFile(SOCKET hSocket);

/**
	@fn void uploadPersonalDBFile(SOCKET hSocket, char* originalHash)
	@brief DogeCloud ���� DB ���� ���ε�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
	@param *originalHash ���� �ؽ���
*/
void uploadPersonalDBFile(SOCKET hSocket, char* originalHash);

/**
	@fn void modifyContacts()
	@brief DogeCloud DB�� ����ó �߰�
	@author �۸۾ƾ߿��غ�
*/
void addContacts();

/**
	@fn void modifyContacts()
	@brief DogeCloud DB�� ����ó ����
	@author �۸۾ƾ߿��غ�
*/
void modifyContacts();

/**
	@fn void deleteContacts()
	@brief DogeCloud DB�� ����ó ����
	@author �۸۾ƾ߿��غ�
*/
void deleteContacts();

/**
	@fn void addMemo()
	@brief DogeCloud DB�� �޸� �߰�
	@author �۸۾ƾ߿��غ�
*/
void addMemo();

/**
	@fn void modifyMemo()
	@brief DogeCloud DB�� �޸� ����
	@author �۸۾ƾ߿��غ�
*/
void modifyMemo();

/**
	@fn void deleteMemo()
	@brief DogeCloud DB�� �޸� ����
	@author �۸۾ƾ߿��غ�
*/
void deleteMemo();

