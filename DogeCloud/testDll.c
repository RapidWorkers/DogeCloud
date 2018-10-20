#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>

#include "../DC_Common/DC_Common.h"

#pragma comment(lib, "DC_Common.lib")
#pragma comment(lib, "ws2_32.lib")

int main() {
	printf("\n============ 2. LEA Library DLL Call Test ============\n");
	testLEA();
	printf("\n\n============ 4. Testing SHA256 Library from DLL ============\n");
	testSHA256();
	printf("\n\n============ 6. Server Connection Test ============\n");
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
		printf("Connected\n");
	}

	//SERVER CONNECTION DONE

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
	printf("���̵�: ");
	scanf_s("%s", uname, 100);
	printf("��й�ȣ: ");
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

	closesocket(hSocket); //���� ���̺귯�� ����
	WSACleanup();

	system("pause");
}