#include "DogeCloud.h"

void userLogin(SOCKET hSocket) {
	//allocating and init memory for packets
	cs_LoginStart LoginStart;
	sc_LoginStartResp LoginStartResp;
	cs_LoginAccountData LoginAccountData;
	sc_LoginDoneResp LoginDoneResp;
	memset(&LoginStart, 0, sizeof(LoginStart)); //init with 0
	memset(&LoginStartResp, 0, sizeof(LoginStartResp));
	memset(&LoginAccountData, 0, sizeof(LoginAccountData));
	memset(&LoginDoneResp, 0, sizeof(LoginDoneResp));


	//constructing LoginStart Packet
	LoginStart.Data.opCode = htonl(OP_CS_LOGINSTART);
	LoginStart.Data.dataLen = htonl(sizeof(cs_LoginStart) - 8);
	LoginStart.Data.clientVersion = htonl(20181020);

	//send Login Start Signal to Server
	if (!sendRaw(hSocket, LoginStart.buf, sizeof(cs_LoginStart), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//get LoginStartResp to server
	if (!recvRaw(hSocket, LoginStartResp.buf, sizeof(LoginStartResp), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//TODO: DROP IF OPCODE NOT MATCH

	//convert numbers big endian to system specific
	LoginStartResp.Data.opCode = ntohl(LoginStartResp.Data.opCode);
	LoginStartResp.Data.dataLen = ntohl(LoginStartResp.Data.dataLen);

	//construct LoginAccountData
	char uname[100] = { 0, }, pwd[100] = { 0, };

	//get username and password from user
	puts("100자를 초과하는 입력은 자동으로 잘립니다.");
	printf("아이디 (최대 99자): ");
	scanf_s("%99s", uname, 100);
	clearStdinBuffer();
	printf("비밀번호 (최대 99자): ");
	scanf_s("%99s", pwd, 100);
	clearStdinBuffer();

	//construct LoginAccountData packet
	LoginAccountData.Data.opCode = htonl(OP_CS_LOGINACCOUNTDATA);
	LoginAccountData.Data.dataLen = htonl(sizeof(cs_LoginAccountData) - 8);
	memcpy(&(LoginAccountData.Data.Username), uname, 100);
	memcpy(&(LoginAccountData.Data.Password), pwd, 100);

	//send Login Data to Server
	if (!sendRaw(hSocket, LoginAccountData.buf, sizeof(LoginAccountData), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//get LoginDoneResp from Server
	if (!recvRaw(hSocket, LoginDoneResp.buf, sizeof(LoginDoneResp), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//TODO: DROP IF OPCODE NOT MATCH

	//convert numbers big endian to system specific
	LoginDoneResp.Data.opCode = ntohl(LoginDoneResp.Data.opCode);
	LoginDoneResp.Data.dataLen = ntohl(LoginDoneResp.Data.dataLen);
	printDebugMsg(DC_DEBUG, DC_ERRORLEVEL, "Got Response: opCode: %d statusCode: %d\n", LoginDoneResp.Data.opCode, LoginDoneResp.Data.statusCode);

	if (LoginDoneResp.Data.statusCode == 0) puts("로그인 실패!!!");
	else if (LoginDoneResp.Data.statusCode == 1)
	{
		puts("로그인 성공!!!");
		loginFlag = 1; //set flag to logged in
		memcpy(sessionKey, LoginDoneResp.Data.sessionKey, 32); //copy sessionKey from Packet
		memcpy(currentUsername, uname, 100); //copy username to currentUsername
		downloadPersonalDBFile(hSocket);
	}
	system("pause");
}

void userLogout(SOCKET hSocket) {

	//allocating and init memory for packets
	cs_LogoutStart LogoutStart;
	sc_LogoutDone LogoutDone;
	memset(&LogoutStart, 0, sizeof(LogoutStart));//init with 0
	memset(&LogoutDone, 0, sizeof(LogoutDone));

	//construct LogoutStart packet
	LogoutStart.Data.opCode = htonl(OP_CS_LOGOUTSTART);
	LogoutStart.Data.dataLen = htonl(sizeof(cs_LogoutStart) - 8);
	sendRaw(hSocket, LogoutStart.buf, sizeof(LogoutStart), 0);

	//get LogoutDone from Server
	if (!recvRaw(hSocket, LogoutDone.buf, sizeof(LogoutDone), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//TODO: DROP IF OPCODE NOT MATCH

	if (LogoutDone.Data.statusCode == 1) {
		puts("로그아웃 성공!");
		memset(sessionKey, 0, sizeof(sessionKey)); //remove sessionKey
		memset(currentUsername, 0, sizeof(currentUsername)); //remove currentUsername
		loginFlag = 0; //set login status flag to not logged in
	}
	else
		puts("로그아웃 실패!");
	system("pause");
}

void userRegister(SOCKET hSocket) {
	//allocating and init memory for packets
	cs_RegisterStart RegisterStart;
	sc_RegisterStartResp RegisterStartResp;
	cs_RegisterAccountData RegisterAccountData;
	sc_RegisterDone RegisterDone;
	memset(&RegisterStart, 0, sizeof(RegisterStart));
	memset(&RegisterStartResp, 0, sizeof(RegisterStartResp));
	memset(&RegisterAccountData, 0, sizeof(RegisterAccountData));
	memset(&RegisterDone, 0, sizeof(RegisterDone));

	//allocating memory for user input
	char email[100] = { 0, };
	char uname[100] = { 0, };
	char pwd[100] = { 0, };

	system("cls");
	printProgramInfo();

	//construct RegisterStart and send packet
	RegisterStart.Data.opCode = htonl(OP_CS_REGISTERSTART);
	RegisterStart.Data.dataLen = htonl(sizeof(RegisterStart) - 8);
	RegisterStart.Data.clientVersion = htonl(20181020);

	if (!sendRaw(hSocket, RegisterStart.buf, sizeof(RegisterStart), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//get RegisterStartResp from Server
	if (!recvRaw(hSocket, RegisterStartResp.buf, sizeof(RegisterStartResp), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}
	
	//convert numbers to host specific
	RegisterStartResp.Data.opCode = ntohl(RegisterStartResp.Data.opCode);
	RegisterStartResp.Data.dataLen = ntohl(RegisterStartResp.Data.dataLen);

	//TODO: DROP IF OPCODE NOT MATCH

	if (RegisterStartResp.Data.statusCode == 0) {//if RelayServer does not allowing reigster
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "현재 회원가입은 불가능합니다. 나중에 다시 시도해 주세요.");
		system("pause");
		return;
	}

	//get username and password from user
	puts("99자를 초과하는 입력은 자동으로 잘립니다.");
	printf("이메일 (최대 99자): ");
	scanf_s("%99s", email, 100);
	clearStdinBuffer();
	printf("아이디 (최대 99자): ");
	scanf_s("%99s", uname, 100);
	clearStdinBuffer();
	printf("비밀번호 (최대 99자): ");
	scanf_s("%99s", pwd, 100);
	clearStdinBuffer();

	//construct RegisterAccountData
	RegisterAccountData.Data.opCode = htonl(OP_CS_REGISTERACCOUNTDATA);
	RegisterAccountData.Data.dataLen = htonl(sizeof(RegisterAccountData) - 8);
	memcpy_s(RegisterAccountData.Data.email, 100, email, 100);
	memcpy_s(RegisterAccountData.Data.Username, 100, uname, 100);
	memcpy_s(RegisterAccountData.Data.Password, 100, pwd, 100);

	if (!sendRaw(hSocket, RegisterAccountData.buf, sizeof(RegisterAccountData), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//get RegisterDone from Server
	if (!recvRaw(hSocket, RegisterDone.buf, sizeof(RegisterDone), 0)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결에 문제가 발생했습니다: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//convert numbers big endian to system specific
	RegisterDone.Data.opCode = ntohl(RegisterDone.Data.opCode);
	RegisterDone.Data.dataLen = ntohl(RegisterDone.Data.dataLen);
	printDebugMsg(DC_DEBUG, DC_ERRORLEVEL, "Got Response: opCode: %d statusCode: %d\n", RegisterDone.Data.opCode, RegisterDone.Data.statusCode);

	switch (RegisterDone.Data.statusCode) {
	case 0:
		puts("회원 가입 실패!!!");
		break;
	case 1:
		puts("아이디 중복!!!");
		break;
	case 2:
		puts("회원가입 및 로그인 성공!!!");
		loginFlag = 1; //set flag to logged in
		memcpy(sessionKey, RegisterDone.Data.sessionKey, 32); //copy sessionKey from Packet
		memcpy(currentUsername, uname, 100); //copy username to currentUsername
		downloadPersonalDBFile(hSocket);
		break;
	default:
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버에서 예상하지 못한 데이터를 전송받았습니다. statusCode: %d", RegisterDone.Data.statusCode);
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
		break;
	}

	system("pause");
}