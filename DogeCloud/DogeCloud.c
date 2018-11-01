#include "DogeCloud.h"

void printMenu() {
	system("cls");
	printProgramInfo();

	if (loginFlag == 0) {
		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. �α���");
		printf_s("\n\t2. �α� �ƿ�");
		printf_s("\n\t3. ����");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");
	}
	else {
		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t4. ���� ���ε� ����");
		printf_s("\n\t5. ���� �ٿ�ε� ����");
		printf_s("\n\t3. ����");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");
	}

}

//session related var
int loginFlag = 0;
char sessionKey[32] = { 0, };

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
		case 4:
			fileUpDemo(hSocket);
			break;
		case 5:
			fileDownDemo(hSocket);
			break;
		default:
			puts("�ùٸ��� ���� �Է��Դϴ�.");
			break;
		}

		if (loginFlag == -1) break;
	}

	closesocket(hSocket); //���� ���̺귯�� ����
	printf("Closed!\n");
	WSACleanup();

	system("pause");
}