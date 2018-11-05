#include "DogeCloud.h"

void login(SOCKET hSocket) {
	int strLen;
	cs_LoginStart LoginStart;
	LoginStart.Data.opCode = htonl(OP_CS_LOGINSTART);
	LoginStart.Data.dataLen = htonl(sizeof(cs_LoginStart) - 8);
	LoginStart.Data.clientVersion = htonl(20181020);
	sendRaw(hSocket, LoginStart.buf, sizeof(cs_LoginStart), 0);

	//get LoginStartResp to server
	sc_LoginStartResp LoginStartResp;
	strLen = recvRaw(hSocket, LoginStartResp.buf, sizeof(LoginStartResp), 0); //recv 함수 호출을 통해서 서버로부터 전송되는 데이터를 수신하고 있다.
	if (!strLen)
		printf("read() error\n");
	LoginStartResp.Data.opCode = ntohl(LoginStartResp.Data.opCode);
	LoginStartResp.Data.dataLen = ntohl(LoginStartResp.Data.dataLen);

	//construct LoginAccountData
	char uname[100] = { 0, }, pwd[100] = { 0, };
	system("pause && cls");
	printProgramInfo();
	puts("100자를 초과하는 입력은 불가능합니다.");
	printf("아이디 (최대 100자): ");
	scanf_s("%s", uname, 100);
	while (getchar() != '\n');
	printf("비밀번호 (최대 100자): ");
	scanf_s("%s", pwd, 100);

	cs_LoginAccountData LoginAccountData;
	LoginAccountData.Data.opCode = htonl(OP_CS_LOGINACCOUNTDATA);
	LoginAccountData.Data.dataLen = htonl(sizeof(cs_LoginAccountData) - 8);
	memcpy(&(LoginAccountData.Data.Username), uname, 100);
	memcpy(&(LoginAccountData.Data.Password), pwd, 100);

	sendRaw(hSocket, LoginAccountData.buf, sizeof(LoginAccountData), 0);

	sc_LoginDoneResp LoginDoneResp;
	strLen = recvRaw(hSocket, LoginDoneResp.buf, sizeof(LoginDoneResp), 0); //recv 함수 호출을 통해서 서버로부터 전송되는 데이터를 수신하고 있다.
	if (!strLen)
		printf("read() error\n");

	LoginDoneResp.Data.opCode = ntohl(LoginDoneResp.Data.opCode);
	LoginDoneResp.Data.dataLen = ntohl(LoginDoneResp.Data.dataLen);
	printf("RCV: opCode: %d sessionKey: %d statusCode: %d\n", LoginDoneResp.Data.opCode, *(LoginDoneResp.Data.sessionKey), LoginDoneResp.Data.statusCode);

	if (LoginDoneResp.Data.statusCode == 0) puts("로그인 실패!!!");
	else if (LoginDoneResp.Data.statusCode == 1)
	{
		puts("로그인 성공!!!");
		loginFlag = 1;
	}
	system("pause");
}

void logout(SOCKET hSocket) {
	cs_LogoutStart LogoutStart;
	int strLen;
	LogoutStart.Data.opCode = htonl(OP_CS_LOGOUTSTART);
	LogoutStart.Data.dataLen = htonl(sizeof(cs_LogoutStart) - 8);
	sendRaw(hSocket, LogoutStart.buf, sizeof(LogoutStart), 0);

	sc_LogoutDone LogoutDone;
	strLen = recvRaw(hSocket, LogoutDone.buf, sizeof(LogoutDone), 0); //recv 함수 호출을 통해서 서버로부터 전송되는 데이터를 수신하고 있다.
	if (!strLen)
		printf("read() error\n");

	if (LogoutDone.Data.statusCode == 1) {
		printf("Logout Done!\n");
		loginFlag = -1;
	}
	else
		printf("Logout Fail!\n");
	system("pause");
}