#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "../DC_Common/DC_Common.h"

#pragma comment(lib, "DC_Common.lib")
#pragma comment(lib, "ws2_32.lib")

void printMenu() {
	system("cls");
	printProgramInfo();
	printf_s("\n\t*********  ��   ��   *********");
	printf_s("\n\t1. �α���");
	printf_s("\n\t2. �α� �ƿ�");
	printf_s("\n\t3. ����");
	printf_s("\n\t******************************");
	printf_s("\n\t�޴� ���� : ");
}

void login(SOCKET hSocket) {
	int strLen;
	cs_LoginStart LoginStart;
	LoginStart.Data.opCode = htonl(OP_CS_LOGINSTART);
	LoginStart.Data.dataLen = htonl(sizeof(cs_LoginStart) - 8);
	LoginStart.Data.clientVersion = htonl(20181020);
	send(hSocket, LoginStart.buf, sizeof(cs_LoginStart), 0);

	//get LoginStartResp to server
	sc_LoginStartResp LoginStartResp;
	strLen = recv(hSocket, LoginStartResp.buf, sizeof(LoginStartResp), 0); //recv �Լ� ȣ���� ���ؼ� �����κ��� ���۵Ǵ� �����͸� �����ϰ� �ִ�.
	if (strLen == -1)
		printf("read() error\n");
	LoginStartResp.Data.opCode = ntohl(LoginStartResp.Data.opCode);
	LoginStartResp.Data.dataLen = ntohl(LoginStartResp.Data.dataLen);

	//construct LoginAccountData
	char uname[100] = { 0, }, pwd[100] = { 0, };
	system("pause && cls");
	printProgramInfo();
	puts("100�ڸ� �ʰ��ϴ� �Է��� �Ұ����մϴ�.");
	printf("���̵� (�ִ� 100��): ");
	scanf_s("%s", uname, 100);
	while (getchar() != '\n');
	printf("��й�ȣ (�ִ� 100��): ");
	scanf_s("%s", pwd, 100);

	cs_LoginAccountData LoginAccountData;
	LoginAccountData.Data.opCode = htonl(OP_CS_LOGINACCOUNTDATA);
	LoginAccountData.Data.dataLen = htonl(sizeof(cs_LoginAccountData) - 8);
	memcpy(&(LoginAccountData.Data.Username), uname, 100);
	memcpy(&(LoginAccountData.Data.Password), pwd, 100);

	send(hSocket, LoginAccountData.buf, sizeof(LoginAccountData), 0);

	sc_LoginDoneResp LoginDoneResp;
	strLen = recv(hSocket, LoginDoneResp.buf, sizeof(LoginDoneResp), 0); //recv �Լ� ȣ���� ���ؼ� �����κ��� ���۵Ǵ� �����͸� �����ϰ� �ִ�.
	if (strLen == -1)
		printf("read() error\n");

	LoginDoneResp.Data.opCode = ntohl(LoginDoneResp.Data.opCode);
	LoginDoneResp.Data.dataLen = ntohl(LoginDoneResp.Data.dataLen);
	printf("RCV: opCode: %d sessionKey: %d statusCode: %d\n", LoginDoneResp.Data.opCode, *(LoginDoneResp.Data.sessionKey), LoginDoneResp.Data.statusCode);

	if (LoginDoneResp.Data.statusCode == 0) puts("�α��� ����!!!");
	else if (LoginDoneResp.Data.statusCode == 1) puts("�α��� ����!!!");
	system("pause");
}

void logout(SOCKET hSocket) {
	cs_LogoutStart LogoutStart;
	int strLen;
	LogoutStart.Data.opCode = htonl(OP_CS_LOGOUTSTART);
	LogoutStart.Data.dataLen = htonl(sizeof(cs_LogoutStart) - 8);
	send(hSocket, LogoutStart.buf, sizeof(LogoutStart), 0);

	sc_LogoutDone LogoutDone;
	strLen = recv(hSocket, LogoutDone.buf, sizeof(LogoutDone), 0); //recv �Լ� ȣ���� ���ؼ� �����κ��� ���۵Ǵ� �����͸� �����ϰ� �ִ�.
	if (strLen == -1)
		printf("read() error\n");

	if (LogoutDone.Data.statusCode == 1)
		printf("Logout Done!\n");
	else
		printf("Logout Fail!\n");
	system("pause");
}

int main() {

	printMenu();

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;
	int strLen;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //���� ���̺귯���� �ʱ�ȭ�ϰ� �ִ�
		printf("WSAStartup() error!\n");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		printf("socket() error\n");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr.s_addr);
	servAddr.sin_port = htons(15754);

	int err = (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR);
	if (err) //������ ������ �������� ������ �����û�� �ϰ� �ִ�
	{
		printf("connect() error!\n");
		printf("code : %d", WSAGetLastError());
	}
	else {
		printf("Successfully Connected\n");
	}

	while (1) {
		int logoutFlag = 0;
		printMenu();
		int select;
		scanf_s("%d", &select);

		switch (select) {
		case 1:
			login(hSocket);
			break;
		case 2:
			logout(hSocket);
			break;
		case 3:
			exit(0);
			break;
		default:
			puts("�ùٸ��� ���� �Է��Դϴ�.");
			break;
		}

		if (logoutFlag) break;
	}

	closesocket(hSocket); //���� ���̺귯�� ����
	printf("Closed!\n");
	WSACleanup();

	system("pause");
}