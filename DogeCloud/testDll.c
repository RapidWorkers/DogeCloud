#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>
#include <Ws2tcpip.h>
#include "../DC_Common/DC_Common.h"

#pragma comment(lib, "DC_Common.lib")
#pragma comment(lib, "ws2_32.lib")

int main() {
	printf("============ 1. DLL Call Test ============\n");
	myFunc();
	printf("\n============ 2. LEA Library DLL Call Test ============\n");
	testLEA();
	printf("\n============ 3. Testing Packet Structure from DLL ============\n");
	testPacketStructure();
	printf("\n\n============ 4. Testing SHA256 Library from DLL ============\n");
	testSHA256();
	printf("\n\n============ 5. Library Include Test ============\n");

	cs_LoginStart test;
	cs_LoginStart temp;

	test.opCode = 139;
	test.clientVersion = 2100000000;

	unsigned char *buf = malloc(sizeof(cs_LoginStart));

	if (buf == NULL) {
		printf("FATAL ERROR! Memory Allocation Failled!!");
		exit(1);
	}

	memcpy(buf, &test, sizeof(cs_LoginStart));
	memcpy(&temp, buf, sizeof(cs_LoginStart));
	test.opCode = 103;

	printf("ORG: opCode: %d clientVersion: %d struct_addr: %p\n", test.opCode, test.clientVersion, &test);
	printf("RST: opCode: %d clientVersion: %d struct_addr: %p\n", temp.opCode, temp.clientVersion, &temp);

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
	send(hSocket, buf, sizeof(test), 0);

	char buf2[sizeof(sc_LoginDoneResp)];
	strLen = recv(hSocket, buf2, sizeof(buf2), 0); //recv �Լ� ȣ���� ���ؼ� �����κ��� ���۵Ǵ� �����͸� �����ϰ� �ִ�.
	if (strLen == -1)
		printf("read() error\n");
	
	sc_LoginDoneResp LoginDoneResp;
	memcpy(&LoginDoneResp, buf2, sizeof(LoginDoneResp));
	printf("RCV: opCode: %d sessionKey: %d statusCode: %d\n", LoginDoneResp.opCode, *(LoginDoneResp.sessionKey), LoginDoneResp.statusCode);

	closesocket(hSocket); //���� ���̺귯�� ����
	WSACleanup();

	system("pause");
}