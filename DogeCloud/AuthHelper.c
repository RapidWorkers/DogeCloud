#include "DogeCloud.h"

void userLogin(SOCKET hSocket) {

	//constructing LoginStart Packet
	cs_LoginStart LoginStart;
	memset(&LoginStart, 0, sizeof(LoginStart)); //init with 0
	LoginStart.Data.opCode = htonl(OP_CS_LOGINSTART);
	LoginStart.Data.dataLen = htonl(sizeof(cs_LoginStart) - 8);
	LoginStart.Data.clientVersion = htonl(20181020);

	if (!sendRaw(hSocket, LoginStart.buf, sizeof(cs_LoginStart), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ���ῡ ������ �߻��߽��ϴ�: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//get LoginStartResp to server
	sc_LoginStartResp LoginStartResp;
	if (!recvRaw(hSocket, LoginStartResp.buf, sizeof(LoginStartResp), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ���ῡ ������ �߻��߽��ϴ�: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//convert numbers big endian to system specific
	LoginStartResp.Data.opCode = ntohl(LoginStartResp.Data.opCode);
	LoginStartResp.Data.dataLen = ntohl(LoginStartResp.Data.dataLen);

	//construct LoginAccountData
	char uname[100] = { 0, }, pwd[100] = { 0, };
	system("pause && cls");
	printProgramInfo();

	//get username and password from user
	puts("100�ڸ� �ʰ��ϴ� �Է��� �ڵ����� �߸��ϴ�.");
	printf("���̵� (�ִ� 99��): ");
	scanf_s("%99s", uname, 100);
	clearStdinBuffer();
	printf("��й�ȣ (�ִ� 99��): ");
	scanf_s("%99s", pwd, 100);
	clearStdinBuffer();

	//construct LoginAccountData packet
	cs_LoginAccountData LoginAccountData;
	memset(&LoginAccountData, 0, sizeof(LoginAccountData)); //init with 0
	LoginAccountData.Data.opCode = htonl(OP_CS_LOGINACCOUNTDATA);
	LoginAccountData.Data.dataLen = htonl(sizeof(cs_LoginAccountData) - 8);
	memcpy(&(LoginAccountData.Data.Username), uname, 100);
	memcpy(&(LoginAccountData.Data.Password), pwd, 100);

	if (!sendRaw(hSocket, LoginAccountData.buf, sizeof(LoginAccountData), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ���ῡ ������ �߻��߽��ϴ�: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//get LoginDoneResp from Server
	sc_LoginDoneResp LoginDoneResp;
	if (!recvRaw(hSocket, LoginDoneResp.buf, sizeof(LoginDoneResp), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ���ῡ ������ �߻��߽��ϴ�: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//convert numbers big endian to system specific
	LoginDoneResp.Data.opCode = ntohl(LoginDoneResp.Data.opCode);
	LoginDoneResp.Data.dataLen = ntohl(LoginDoneResp.Data.dataLen);
	printDebugMsg(DC_DEBUG, DC_ERRORLEVEL, "Got Response: opCode: %d statusCode: %d\n", LoginDoneResp.Data.opCode, LoginDoneResp.Data.statusCode);

	if (LoginDoneResp.Data.statusCode == 0) puts("�α��� ����!!!");
	else if (LoginDoneResp.Data.statusCode == 1)
	{
		puts("�α��� ����!!!");
		loginFlag = 1; //set flag to logged in
		memcpy(sessionKey, LoginDoneResp.Data.sessionKey, 32); //copy sessionKey from Packet
		memcpy(currentUsername, uname, 100); //copy username to currentUsername
	}
	system("pause");
}

void userLogout(SOCKET hSocket) {

	//construct LogoutStart packet
	cs_LogoutStart LogoutStart;
	LogoutStart.Data.opCode = htonl(OP_CS_LOGOUTSTART);
	LogoutStart.Data.dataLen = htonl(sizeof(cs_LogoutStart) - 8);
	sendRaw(hSocket, LogoutStart.buf, sizeof(LogoutStart), 0);

	//get LogoutDone from Server
	sc_LogoutDone LogoutDone;
	if (!recvRaw(hSocket, LogoutDone.buf, sizeof(LogoutDone), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ���ῡ ������ �߻��߽��ϴ�: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	if (LogoutDone.Data.statusCode == 1) {
		puts("�α׾ƿ� ����!");
		memset(sessionKey, 0, sizeof(sessionKey)); //remove sessionKey
		memset(currentUsername, 0, sizeof(currentUsername)); //remove currentUsername
		loginFlag = 0; //set login status flag to not logged in
	}
	else
		puts("�α׾ƿ� ����!");
	system("pause");
}

void userRegister(SOCKET hSocket) {

}