#include "RelayServer.h"

void procLoginStart(SOCKET hClientSock) {

	unsigned char rcvLen[4];
	int strLen = recv(hClientSock, rcvLen, 4, 0);

	unsigned long rcvLenLong;
	memcpy(&rcvLenLong, rcvLen, 4);
	rcvLenLong = ntohl(rcvLenLong);

	if (!strLen || sizeof(cs_LoginStart)-8 != rcvLenLong)//if size mismatch..
		return;//drop

	//Get LoginStart from client
	cs_LoginStart LoginStart;
	LoginStart.Data.opCode = OP_CS_LOGINSTART;
	LoginStart.Data.dataLen = sizeof(cs_LoginStart) - 8;
	recv(hClientSock, LoginStart.buf + 8, sizeof(cs_LoginStart) - 8, 0);

	//TODO: LOGIN STATUS CHANGE TO 1

	//Sending LoginStartResp to Client
	sc_LoginStartResp LoginStartResp;
	LoginStartResp.Data.opCode = htonl(OP_SC_LOGINSTARTRESP);
	LoginStartResp.Data.dataLen = htonl(sizeof(sc_LoginStartResp) - 8);
	LoginStartResp.Data.statusCode = 1;
	send(hClientSock, LoginStartResp.buf, sizeof(LoginStartResp), 0);
}

void procLoginAccountData(SOCKET hClientSock) {
	//Get LoginAccountData from client
	unsigned char rcvLen[4];
	int strLen = recv(hClientSock, rcvLen, 4, 0);

	unsigned long rcvLenLong;
	memcpy(&rcvLenLong, rcvLen, 4);
	rcvLenLong = ntohl(rcvLenLong);

	if (!strLen || sizeof(cs_LoginAccountData) - 8 != rcvLenLong)//if size mismatch..
		return;//drop

	//get LoginAccountData from client
	cs_LoginAccountData LoginAccountData;
	LoginAccountData.Data.opCode = OP_CS_LOGINACCOUNTDATA;
	LoginAccountData.Data.dataLen = sizeof(cs_LoginAccountData) - 8;
	strLen = recv(hClientSock, LoginAccountData.buf+8, sizeof(LoginAccountData)-8, 0);

	//construct LoginDoneResp
	sc_LoginDoneResp LoginDoneResp;
	memset(&LoginDoneResp, 0, sizeof(sc_LoginDoneResp));
	LoginDoneResp.Data.opCode = htonl(OP_SC_LOGINDONERESP);
	LoginDoneResp.Data.dataLen = htonl(sizeof(sc_LoginDoneResp) - 8);

	//temporary set data for login
	char hashedPassword[32];
	char tempUsrName[] = "test";
	char tempUsrPwdHashed[32];
	SHA256_Text("1234", tempUsrPwdHashed);
	SHA256_Text(LoginAccountData.Data.Password, hashedPassword);

	if (!strcmp(tempUsrName, LoginAccountData.Data.Username)) {
		int flag = 1;
		for (int i = 0; i < 32; i++) {
			if (tempUsrPwdHashed[i] != hashedPassword[i]) {
				flag = 0;
				break;
			}
		}

		if (flag) {//ID and PWD Match
			printDebugMsg(1, ERLEVEL, "ACCOUNT MATCH");
			for (int i = 0; i < 32; i++)
				LoginDoneResp.Data.sessionKey[i] = i;
			LoginDoneResp.Data.statusCode = 1;
		}
		else {//not match
			printDebugMsg(1, ERLEVEL, "ACCOUNT NOT MATCH");
			LoginDoneResp.Data.statusCode = 0;
		}
	}
	else {
		printDebugMsg(1, ERLEVEL, "ACCOUNT NOT EXIST");
		LoginDoneResp.Data.statusCode = 0;
	}

	send(hClientSock, LoginDoneResp.buf, sizeof(LoginDoneResp), 0);
	printDebugMsg(1, ERLEVEL, "Sent LoginDoneResp to Client");
}

void procLogout(SOCKET hClientSock) {
	cs_LogoutStart LogoutStart;
	sc_LogoutDone LogoutDone;

	recv(hClientSock, LogoutStart.buf + 4, sizeof(LogoutStart) - 4, 0);

	LogoutDone.Data.opCode = htonl(OP_SC_LOGOUTDONE);
	LogoutDone.Data.dataLen = htonl(1);
	LogoutDone.Data.statusCode = 1;

	send(hClientSock, LogoutDone.buf, sizeof(LogoutDone), 0);
	closesocket(hClientSock);
}

void procRegister(SOCKET hClientSock) {
	//will be implemented
}