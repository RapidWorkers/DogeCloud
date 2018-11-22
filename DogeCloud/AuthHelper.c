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
	@file AuthHelper.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief DogeCloud �α��� �α׾ƿ� ���� �Լ� ����
*/

#include "DogeCloud.h"

/**
	@fn void userLogin(SOCKET hSocket)
	@brief DogeCloud �߰� ���� �α���
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void userLogin(SOCKET hSocket) {
	//��Ŷ�� ������ ���� �Ҵ�
	cs_LoginStart LoginStart;
	sc_LoginStartResp LoginStartResp;
	cs_LoginAccountData LoginAccountData;
	sc_LoginDoneResp LoginDoneResp;

	//0���� �ʱ�ȭ
	memset(&LoginStart, 0, sizeof(LoginStart));
	memset(&LoginStartResp, 0, sizeof(LoginStartResp));
	memset(&LoginAccountData, 0, sizeof(LoginAccountData));
	memset(&LoginDoneResp, 0, sizeof(LoginDoneResp));

	//���� ���̵� �� ��й�ȣ ����� �迭 �Ҵ� �� �ʱ�ȭ
	char uname[100] = { 0, }, pwd[100] = { 0, };

	//��Ŷ ����
	LoginStart.Data.opCode = htonl(OP_CS_LOGINSTART);
	LoginStart.Data.dataLen = htonl(sizeof(cs_LoginStart) - 8);
	LoginStart.Data.clientVersion = htonl(20181020);

	//�α��� ���� ��Ŷ ����
	sendData(hSocket, LoginStart.buf, sizeof(cs_LoginStart), 0);

	//�α��� ���� ��Ŷ �ޱ�
	recvData(hSocket, LoginStartResp.buf, sizeof(sc_LoginStartResp), 0);

	//���޹��� ���ڸ� Big Endian���� ȣ��Ʈ ȯ�� �ε������ ����
	LoginStartResp.Data.opCode = ntohl(LoginStartResp.Data.opCode);
	LoginStartResp.Data.dataLen = ntohl(LoginStartResp.Data.dataLen);

	//����ڿ��Լ� ���̵�� ��й�ȣ ������
	puts("100�ڸ� �ʰ��ϴ� �Է��� �ڵ����� �߸��ϴ�.");
	printf("���̵� (�ִ� 99��): ");
	scanf_s("%99s", uname, 100);
	clearStdinBuffer();
	printf("��й�ȣ (�ִ� 99��): ");
	scanf_s("%99s", pwd, 100);
	clearStdinBuffer();

	//�α��� ���� ��Ŷ �ʱ�ȭ
	LoginAccountData.Data.opCode = htonl(OP_CS_LOGINACCOUNTDATA);
	LoginAccountData.Data.dataLen = htonl(sizeof(cs_LoginAccountData) - 8);
	memcpy(&(LoginAccountData.Data.Username), uname, 100);
	memcpy(&(LoginAccountData.Data.Password), pwd, 100);

	//�α��� ���� ���� 
	sendData(hSocket, LoginAccountData.buf, sizeof(LoginAccountData), 0)

	//�α��� �Ϸ� ��Ŷ �������� ������
	recvData(hSocket, LoginDoneResp.buf, sizeof(LoginDoneResp), 0)

	//���޹��� ���ڸ� Big Endian���� ȣ��Ʈ ȯ�� �ε������ ����
	LoginDoneResp.Data.opCode = ntohl(LoginDoneResp.Data.opCode);
	LoginDoneResp.Data.dataLen = ntohl(LoginDoneResp.Data.dataLen);

	if (LoginDoneResp.Data.statusCode == 0) puts("�α��� ����!!!");//�α��� ���н�
	else if (LoginDoneResp.Data.statusCode == 1)//�α��� ������
	{
		puts("�α��� ����!!!");
		loginFlag = 1; //�α��� �÷��� ����
		memcpy(sessionKey, LoginDoneResp.Data.sessionKey, 32); //����Ű�� ��Ŷ���� ���������� ����
		memcpy(currentUsername, uname, 100); //���̵� ���������� ����
		downloadPersonalDBFile(hSocket);//���� DB ������ �������� �ٿ�ε�
	}
	else {//�� ���� ���� ���޹��� ���
		printDebugMsg(DC_ERROR, errorLevel, "�߰輭������ �������� ���� �����͸� ���۹޾ҽ��ϴ�. statusCode: %d", LoginDoneResp.Data.statusCode);
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	system("pause");
	return;
}

/**
	@fn void userLogout(SOCKET hSocket)
	@brief DogeCloud �߰� ���� �α׾ƿ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void userLogout(SOCKET hSocket) {

	//��Ŷ�� ���� �޸� �Ҵ� �� �ʱ�ȭ
	cs_LogoutStart LogoutStart;
	sc_LogoutDone LogoutDone;
	memset(&LogoutStart, 0, sizeof(LogoutStart));//0���� �ʱ�ȭ
	memset(&LogoutDone, 0, sizeof(LogoutDone));

	//�α׾ƿ� ���� ��Ŷ ����
	LogoutStart.Data.opCode = htonl(OP_CS_LOGOUTSTART);
	LogoutStart.Data.dataLen = htonl(sizeof(cs_LogoutStart) - 8);

	//�α׾ƿ� ���� ��Ŷ ����
	sendData(hSocket, LogoutStart.buf, sizeof(LogoutStart), 0);

	//�������� �α��� �Ϸ� ��Ŷ �޾ƿ�
	recvData(hSocket, LogoutDone.buf, sizeof(LogoutDone), 0);

	if (LogoutDone.Data.statusCode == 1) {//�α׾ƿ� ������
		puts("�α׾ƿ� ����!");
		memset(sessionKey, 0, sizeof(sessionKey)); //����Ű ������������ ����
		memset(currentUsername, 0, sizeof(currentUsername)); //���̵� ����
		loginFlag = 0; //�α��� �÷��� ����
	}
	else//���� �Ǵ� �� ���� �����
		puts("�α׾ƿ� ����!");

	system("pause");
	return;
}

/**
	@fn void userRegister(SOCKET hSocket)
	@brief DogeCloud ȸ������
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void userRegister(SOCKET hSocket) {
	//��Ŷ�� ���� �޸� �Ҵ� �� �ʱ�ȭ
	cs_RegisterStart RegisterStart;
	sc_RegisterStartResp RegisterStartResp;
	cs_RegisterAccountData RegisterAccountData;
	sc_RegisterDone RegisterDone;
	memset(&RegisterStart, 0, sizeof(RegisterStart));//0���� �ʱ�ȭ
	memset(&RegisterStartResp, 0, sizeof(RegisterStartResp));
	memset(&RegisterAccountData, 0, sizeof(RegisterAccountData));
	memset(&RegisterDone, 0, sizeof(RegisterDone));

	//���� �Է��� ���� �迭 ���� �� �ʱ�ȭ
	char email[100] = { 0, };
	char uname[100] = { 0, };
	char pwd[100] = { 0, };

	system("cls");
	printProgramInfo();

	//ȸ������ ���� ��Ŷ ����
	RegisterStart.Data.opCode = htonl(OP_CS_REGISTERSTART);
	RegisterStart.Data.dataLen = htonl(sizeof(RegisterStart) - 8);
	RegisterStart.Data.clientVersion = htonl(20181020);

	//ȸ������ ���� ��Ŷ ����
	sendData(hSocket, RegisterStart.buf, sizeof(RegisterStart), 0);

	//ȸ������ ���� ���� ��Ŷ �������� �޾ƿ�
	recvData(hSocket, RegisterStartResp.buf, sizeof(RegisterStartResp), 0)
	
	//���޹��� ���ڸ� Big Endian���� ȣ��Ʈ ȯ�� �ε������ ����
	RegisterStartResp.Data.opCode = ntohl(RegisterStartResp.Data.opCode);
	RegisterStartResp.Data.dataLen = ntohl(RegisterStartResp.Data.dataLen);

	if (RegisterStartResp.Data.statusCode == 0) {//���� ȸ������ �Ұ� ���¶��
		printDebugMsg(DC_ERROR, errorLevel, "���� ȸ�������� �Ұ����մϴ�. ���߿� �ٽ� �õ��� �ּ���.");
		system("pause");
		return;
	}

	//����ڷκ��� �̸���, ���̵�, ��й�ȣ �޾ƿ�
	puts("99�ڸ� �ʰ��ϴ� �Է��� �ڵ����� �߸��ϴ�.");
	printf("�̸��� (�ִ� 99��): ");
	scanf_s("%99s", email, 100);
	clearStdinBuffer();
	printf("���̵� (�ִ� 99��): ");
	scanf_s("%99s", uname, 100);
	clearStdinBuffer();
	printf("��й�ȣ (�ִ� 99��): ");
	scanf_s("%99s", pwd, 100);
	clearStdinBuffer();

	//ȸ������ ���� ��Ŷ ����
	RegisterAccountData.Data.opCode = htonl(OP_CS_REGISTERACCOUNTDATA);
	RegisterAccountData.Data.dataLen = htonl(sizeof(RegisterAccountData) - 8);
	memcpy_s(RegisterAccountData.Data.email, 100, email, 100);
	memcpy_s(RegisterAccountData.Data.Username, 100, uname, 100);
	memcpy_s(RegisterAccountData.Data.Password, 100, pwd, 100);

	//ȸ������ ���� ��Ŷ ����
	sendData(hSocket, RegisterAccountData.buf, sizeof(RegisterAccountData), 0);

	//ȸ������ �Ϸ� ��Ŷ ������
	recvData(hSocket, RegisterDone.buf, sizeof(RegisterDone), 0);

	//���޹��� ���ڸ� Big Endian���� ȣ��Ʈ ȯ�� �ε������ ����
	RegisterDone.Data.opCode = ntohl(RegisterDone.Data.opCode);
	RegisterDone.Data.dataLen = ntohl(RegisterDone.Data.dataLen);

	switch (RegisterDone.Data.statusCode) {//ȸ������ ���� ������ ���� �б�
	case 0://���н�
		puts("ȸ�� ���� ����!!!");
		break;
	case 1://���̵� �ߺ�
		puts("���̵� �ߺ�!!!");
		break;
	case 2://����
		puts("ȸ������ �� �α��� ����!!!");
		loginFlag = 1; //�α��� �÷��� ����
		memcpy(sessionKey, RegisterDone.Data.sessionKey, 32); //���������� ����Ű ����
		memcpy(currentUsername, uname, 100); //���������� ���̵� ����
		downloadPersonalDBFile(hSocket); //���� DB ���� �������� �ٿ�ε�
		break;
	default://�� �� ����
		printDebugMsg(DC_ERROR, errorLevel, "�߰輭������ �������� ���� �����͸� ���۹޾ҽ��ϴ�. statusCode: %d", RegisterDone.Data.statusCode);
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
		break;
	}

	system("pause");
	return;
}