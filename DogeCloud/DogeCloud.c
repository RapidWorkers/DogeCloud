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
	@file DogeCloud.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief DogeCloud Main ����
*/

#include "DogeCloud.h"

/**
	@fn void printMenu()
	@brief DogeCloud ���� �޴� ���
	@author �۸۾ƾ߿��غ�
*/
void printMenu() {
	system("cls");
	printProgramInfo();

	switch (loginFlag) {//Login ���¿� ���� �޴� ǥ��
	case 0://������ �α��� ���� ���� ����
		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. �α���");
		printf_s("\n\t2. ȸ�� ����");
		printf_s("\n\t3. ����");
		printf_s("\n\t4. ���α׷� ���� �� ���̼���");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");
		break;
	case 1://������ �α��� ���� ���
		printf_s("\n���� �α��� �����: %s", currentUsername);
		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. �޸� ����");
		printf_s("\n\t2. ����ó ����");
		printf_s("\n\t3. ���� ����");
		printf_s("\n\t4. �α׾ƿ�");
		printf_s("\n\t5. ����");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");
		break;
	default://��ȿ���� ���� �α��� ������ ��
		printDebugMsg(DC_ERROR, errorLevel, "��ȿ���� ���� LoginFlag �����Դϴ�. �޸� ���� �ǽ�!!\n");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�..");
		system("pause");
		exit(1);
		break;
	}
}

/**
	@fn int initProgram(WSADATA *wsaData, SOCKET *hRelayServSocket, SOCKADDR_IN *RelayServAddr)
	@brief DogeCloud �߰� ���� ����
	@author �۸۾ƾ߿��غ�
	@param *wsaData WSADATA ����ü �ּ�
	@param *hRelayServSocket SOCKET ����ü �ּ�
	@param *RelayServAddr SOCKADDR_IN ����ü �ּ�
	@return 0 = ����, 1 = ����
*/
int initProgram(WSADATA *wsaData, SOCKET *hRelayServSocket, SOCKADDR_IN *RelayServAddr) {
	printProgramInfo();

	//���ε� ���� ����
	if (!CreateDirectory("./upload", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {//�̹� ������ ������ ���� ��츦 ������ ��� ������ ���α׷� ����
			printDebugMsg(DC_ERROR, errorLevel, "Create Directory Fail");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}
	};

	//�ٿ�ε� ���� ����
	if (!CreateDirectory("./download", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {//�̹� ������ ������ ���� ��츦 ������ ��� ������ ���α׷� ����
			printDebugMsg(DC_ERROR, errorLevel, "Create Directory Fail");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}
	};

	if (WSAStartup(MAKEWORD(2, 2), wsaData) != 0) { //���� ���̺귯�� �ʱ�ȭ
		printDebugMsg(DC_ERROR, errorLevel, "WSAStartup ����!\n");
		return 0;
	}

	*hRelayServSocket = socket(PF_INET, SOCK_STREAM, 0); //���� �ʱ�ȭ
	if (*hRelayServSocket == INVALID_SOCKET) {//���� ���� �����ߴ��� �˻�
		printDebugMsg(DC_ERROR, errorLevel, "��ȿ���� ���� �����Դϴ�.\n");
		return 0;
	}

	readRelayServerPath(RelayServAddr); //�߰輭�� �ּ� �������Ͽ��� �о��

	int err = (connect(*hRelayServSocket, (SOCKADDR*)RelayServAddr, sizeof(*RelayServAddr)) == SOCKET_ERROR);//�߰輭���� ����
	if (err) //���� �˻�
	{
		printDebugMsg(DC_ERROR, errorLevel, "�߰輭������ ������ �����߽��ϴ�: %d", WSAGetLastError());
		return 0;
	}

	printDebugMsg(DC_INFO, errorLevel, "���������� �߰輭���� ����Ǿ����ϴ�.\n");
	Sleep(500);
	return 1;
}

//���� ���� ���� ����
/**
	@var int loginFlag
	�α��� ���� ����
*/
int loginFlag = 0;

/**
	@var char currentUsername[100]
	���� �α��ε� ���� �̸� ����
*/
char sessionKey[32] = { 0, };

/**
	@var char sessionKey[32]
	�߰輭�� ���� ����Ű ����� ����
*/
char currentUsername[100] = { 0, };

/**
	@var int errorLevel
	�߰輭�� ���� ����Ű ����� ����
*/
int errorLevel;

/**
	@fn int main()
	@brief DogeCloud ������(Main)
	@author �۸۾ƾ߿��غ�
*/
int main() {

	/** @brief ���̺귯���� ����ü */
	WSADATA wsaData;

	/** @brief ���� ����� ���� ����ü */
	SOCKET hRelayServSocket;

	/** @brief ���� �ּ� �����ϴ� ����ü */
	SOCKADDR_IN RelayServAddr;

	//�������� �а� ����
	setErrorLevel();

	if (!initProgram(&wsaData, &hRelayServSocket, &RelayServAddr)) {//initProgram �Լ��� �ʱ�ȭ �� ����
		printDebugMsg(DC_ERROR, errorLevel, "���α׷� �ʱ�ȭ ����");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	printMenu();

	while (1) {//�޴� ������ �����Ҷ����� ���ѹݺ�
		printMenu();

		int select;//���� �Է� �����
		scanf_s("%d", &select);
		clearStdinBuffer();

		//�Է� �ް� ȭ�� ����
		system("cls");
		printProgramInfo();

		if (loginFlag == 0) {//�α����� ���ߴٸ�
			switch (select) {
			case 1: //�α���
				userLogin(hRelayServSocket);
				break;
			case 2: //ȸ�� ����
				userRegister(hRelayServSocket);
				break;
			case 3: //����
				closesocket(hRelayServSocket); //���� ���̺귯�� ����
				WSACleanup();
				exit(0);
				break;
			case 4: //���α׷� ���̼��� ǥ��
				printProgramInfo();
				printLicense();
				system("pause");
				break;
			default: //��ȿ���� ���� �Է�
				printDebugMsg(DC_WARN, errorLevel, "�ùٸ��� ���� �Է��Դϴ�.");
				Sleep(1000);
				break;
			}
		}
		else if (loginFlag == 1) {//�α����� �ߴٸ�
			switch (select) {
			case 1: //�޸� ����
				manageMemo(hRelayServSocket);
				break;
			case 2: //����ó ����
				manageContacts(hRelayServSocket);
				break;
			case 3: //���� ����
				manageFile(hRelayServSocket);
				break;
			case 4: //�α׾ƿ�
				userLogout(hRelayServSocket);
				break;
			case 5: //����
				userLogout(hRelayServSocket);//���� �α׾ƿ�
				closesocket(hRelayServSocket); //���� ����
				printf_s("���� ���� ����!\n");
				WSACleanup();//���� ���̺귯�� ����
				system("pause");
				exit(0);//����
				break;
			default://�ùٸ��� ���� �Է�
				printDebugMsg(DC_WARN, errorLevel, "�ùٸ��� ���� �Է��Դϴ�.");
				Sleep(1000);
				break;
			}
		}
	}

	closesocket(hRelayServSocket); //���� ���� ����
	printf_s("���� ���� ����!\n");
	WSACleanup();//���� ���̺귯�� ����
	system("pause");

	return 0;
}