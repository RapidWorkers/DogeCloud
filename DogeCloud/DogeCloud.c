#include "DogeCloud.h"

void printMenu() {
	system("cls");
	printProgramInfo();

	if (loginFlag == 0) {
		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 로그인");
		printf_s("\n\t2. 로그 아웃");
		printf_s("\n\t3. 종료");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");
	}
	else {
		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t4. 파일 업로드 데모");
		printf_s("\n\t5. 파일 다운로드 데모");
		printf_s("\n\t3. 종료");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");
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

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) //소켓 라이브러리를 초기화하고 있다
		printf("WSAStartup() error!\n");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		printf("socket() error\n");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	inet_pton(AF_INET, "127.0.0.1", &servAddr.sin_addr.s_addr);
	servAddr.sin_port = htons(15754);

	int err = (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR);
	if (err) //생성된 소켓을 바탕으로 서버에 연결요청을 하고 있다
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
			puts("올바르지 않은 입력입니다.");
			break;
		}

		if (loginFlag == -1) break;
	}

	closesocket(hSocket); //소켓 라이브러리 해제
	printf("Closed!\n");
	WSACleanup();

	system("pause");
}