#include "RelayServer.h"

void procLoginStart(SOCKET hClientSock) {
	//Get LoginStart from client
	cs_LoginStart LoginStart;
	LoginStart.Data.opCode = OP_CS_LOGINSTART;
	LoginStart.Data.dataLen = sizeof(cs_LoginStart) - 8;
	recvData(hClientSock, LoginStart.buf + 4, sizeof(cs_LoginStart) - 4, 0);

	//TODO: LOGIN STATUS CHANGE TO 1

	//Sending LoginStartResp to Client
	sc_LoginStartResp LoginStartResp;
	LoginStartResp.Data.opCode = htonl(OP_SC_LOGINSTARTRESP);
	LoginStartResp.Data.dataLen = htonl(sizeof(sc_LoginStartResp) - 8);
	LoginStartResp.Data.statusCode = 1;

	sendData(hClientSock, LoginStartResp.buf, sizeof(LoginStartResp), 0);
	doLogin(hClientSock);
	return;
}

void doLogin(SOCKET hClientSock) {
	
	cs_LoginAccountData LoginAccountData;
	sc_LoginDoneResp LoginDoneResp;
	memset(&LoginAccountData, 0, sizeof(cs_LoginAccountData));
	memset(&LoginDoneResp, 0, sizeof(sc_LoginDoneResp));

	//get LoginAccountData from client
	recvData(hClientSock, LoginAccountData.buf, sizeof(LoginAccountData), 0);

	LoginAccountData.Data.opCode = ntohl(LoginAccountData.Data.opCode);
	LoginAccountData.Data.dataLen = ntohl(LoginAccountData.Data.dataLen);

	//construct LoginDoneResp
	LoginDoneResp.Data.opCode = htonl(OP_SC_LOGINDONERESP);
	LoginDoneResp.Data.dataLen = htonl(sizeof(sc_LoginDoneResp) - 8);

	//prepare password for login auth
	unsigned char hashedPassword[32];
	SHA256_Text(LoginAccountData.Data.Password, hashedPassword);
	unsigned char hashedPasswordText[65] = { 0, };
	for (int i = 0; i < 32; i++) {
		sprintf_s(hashedPasswordText + (2 * i), 3, "%02x", hashedPassword[i]);
	}

	//binding data for mysql prepared statement
	MYSQL_STMT *stmt = NULL;
	MYSQL_BIND bind[2];

	char *query = "SELECT count(id), id FROM accounts WHERE username = ? and pwd = ?;";

	memset(bind, 0, sizeof(bind)); //init bind

	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = LoginAccountData.Data.Username;
	bind[0].buffer_length = (unsigned long)strlen(LoginAccountData.Data.Username);

	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = hashedPasswordText;
	bind[1].buffer_length = 64;

	int loginFlag = 0;
	int userUID = 0;

	MYSQL_BIND bind_result[2];
	memset(&bind_result, 0, sizeof(bind_result)); //init bind
	bind_result[0].buffer_type = MYSQL_TYPE_LONG;
	bind_result[0].buffer = &loginFlag;
	bind_result[0].buffer_length = 4;
	bind_result[1].buffer_type = MYSQL_TYPE_LONG;
	bind_result[1].buffer = &userUID;
	bind_result[1].buffer_length = 4;

	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	sqlPrepareAndExecute(&sqlHandle, stmt, query, bind, &bind_result);

	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (loginFlag == 1) {//ID and PWD Match
		GenerateSessionKey(LoginDoneResp.Data.sessionKey);

		char tmp[65] = { 0, };
		for (int i = 0; i < 32; i++)
			sprintf_s(tmp + (2 * i), 3, "%02x", LoginDoneResp.Data.sessionKey[i]);
		printDebugMsg(DC_DEBUG, DC_ERRORLEVEL, "Generated Session Key: %s", tmp);

		WaitForSingleObject(hMutex, INFINITE);//to protect global var access
		for (int i = 0; i < clientCount; i++) {
			if (hClientSock == hClientSocks[i]) {
				memcpy(sessionList[i].sessionKey, LoginDoneResp.Data.sessionKey, 32); //copy generated sessionKey
				sessionList[i].userUID = userUID;
				sessionList[i].currentStatus = 1;
				break;
			}
		}
		ReleaseMutex(hMutex);

		LoginDoneResp.Data.statusCode = 1;
	}
	else {
		LoginDoneResp.Data.statusCode = 0;
	}

	sendData(hClientSock, LoginDoneResp.buf, sizeof(LoginDoneResp), 0);
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Sent LoginDoneResp to Client");

	mysql_stmt_close(stmt);
}

void procLogout(SOCKET hClientSock) {
	cs_LogoutStart LogoutStart;
	sc_LogoutDone LogoutDone;

	recvData(hClientSock, LogoutStart.buf + 4, sizeof(LogoutStart) - 4, 0);

	LogoutDone.Data.opCode = htonl(OP_SC_LOGOUTDONE);
	LogoutDone.Data.dataLen = htonl(1);
	LogoutDone.Data.statusCode = 1;

	//closed session is automatically handled by socketHandler so we don't have to release any globar var here

	sendData(hClientSock, LogoutDone.buf, sizeof(LogoutDone), 0);

}

void procRegisterStart(SOCKET hClientSock) {
	//allocating and init memory for packets
	cs_RegisterStart RegisterStart;
	sc_RegisterStartResp RegisterStartResp;
	memset(&RegisterStart, 0, sizeof(RegisterStart));
	memset(&RegisterStartResp, 0, sizeof(RegisterStartResp));

	RegisterStart.Data.opCode = OP_CS_LOGINACCOUNTDATA;
	RegisterStart.Data.dataLen = sizeof(cs_LoginAccountData) - 8;

	recvData(hClientSock, RegisterStart.buf + 4, sizeof(RegisterStart) - 4, 0)

	//TODO: DROP IF OPCODE NOT MATCH

	RegisterStartResp.Data.opCode = htonl(OP_SC_REGISTERSTARTRESP);
	RegisterStartResp.Data.dataLen = htonl(sizeof(RegisterStartResp) - 8);
	RegisterStartResp.Data.statusCode = 1;

	sendData(hClientSock, RegisterStartResp.buf, sizeof(RegisterStartResp), 0);

	doRegister(hClientSock);

	return;
}

void doRegister(SOCKET hClientSock) {
	cs_RegisterAccountData RegisterAccountData;
	sc_RegisterDone RegisterDone;

	recvData(hClientSock, RegisterAccountData.buf, sizeof(RegisterAccountData), 0);

	char* unameDupChkQuery = "SELECT count(id) FROM accounts WHERE username = ?;";
	char* insertQuery = "INSERT into accounts VALUES(NULL, ?, ?, ?);";
	char* getUID = "SELECT id FROM accounts WHERE username = ?;";

	MYSQL_STMT *stmt = NULL;
	MYSQL_BIND query1_bind[1];
	MYSQL_BIND query2_bind[3];
	memset(query1_bind, 0, sizeof(query1_bind)); //init bind
	memset(query2_bind, 0, sizeof(query2_bind));

	query1_bind[0].buffer_type = MYSQL_TYPE_STRING;
	query1_bind[0].buffer = RegisterAccountData.Data.Username;
	query1_bind[0].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.Username);
	//bind[0].is_null = is_null;
	//bind[0].length = &usrNameLen; //why this is needed?

	int duplicateFlag = 0;

	MYSQL_BIND bind1_result;
	memset(&bind1_result, 0, sizeof(bind1_result)); //init bind
	bind1_result.buffer_type = MYSQL_TYPE_LONG;
	bind1_result.buffer = &duplicateFlag;
	bind1_result.buffer_length = 4;

	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	sqlPrepareAndExecute(&sqlHandle, stmt, unameDupChkQuery, query1_bind, &bind1_result);

	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (duplicateFlag) {
		RegisterDone.Data.opCode = htonl(OP_SC_REGISTERDONE);
		RegisterDone.Data.dataLen = htonl(sizeof(RegisterDone) - 8);
		RegisterDone.Data.statusCode = 1;

		if (!sendRaw(hClientSock, RegisterDone.buf, sizeof(RegisterDone), 0)) {
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Connection Error: %d", WSAGetLastError());
			return;
		}
		mysql_stmt_free_result(stmt);
		return;
	}

	mysql_stmt_free_result(stmt);//ready for next query

	//hash password for register
	unsigned char hashedPassword[32];
	SHA256_Text(RegisterAccountData.Data.Password, hashedPassword);
	unsigned char hashedPasswordText[65] = { 0, };
	for (int i = 0; i < 32; i++) {
		sprintf_s(hashedPasswordText + (2 * i), 3, "%02x", hashedPassword[i]);
	}

	query2_bind[0].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[0].buffer = RegisterAccountData.Data.Username;
	query2_bind[0].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.Username);

	query2_bind[1].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[1].buffer = hashedPasswordText;
	query2_bind[1].buffer_length = 64;
	
	query2_bind[2].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[2].buffer = RegisterAccountData.Data.email;
	query2_bind[2].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.email);

	//DO REGISTER

	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	sqlPrepareAndExecute(&sqlHandle, stmt, insertQuery, query2_bind, NULL);
	mysql_stmt_free_result(stmt);//ready for next query

	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	int userUID = 0;
	MYSQL_BIND bind_result3;
	memset(&bind_result3, 0, sizeof(bind_result3)); //init bind
	bind_result3.buffer_type = MYSQL_TYPE_LONG;
	bind_result3.buffer = &userUID;
	bind_result3.buffer_length = 4;

	sqlPrepareAndExecute(&sqlHandle, stmt, getUID, query1_bind, &bind_result3);

	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SEND RESULT
	RegisterDone.Data.opCode = htonl(OP_SC_REGISTERDONE);
	RegisterDone.Data.dataLen = htonl(sizeof(RegisterDone) - 8);
	RegisterDone.Data.statusCode = 2;
	GenerateSessionKey(RegisterDone.Data.sessionKey);
	char tmp[65] = { 0, };
	for (int i = 0; i < 32; i++)
		sprintf_s(tmp + (2 * i), 3, "%02x", RegisterDone.Data.sessionKey[i]);
	printDebugMsg(DC_DEBUG, DC_ERRORLEVEL, "Generated Session Key: %s", tmp);

	WaitForSingleObject(hMutex, INFINITE);//to protect global var access
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			memcpy(sessionList[i].sessionKey, RegisterDone.Data.sessionKey, 32); //copy generated sessionKey
			sessionList[i].userUID = userUID;
			sessionList[i].currentStatus = 1;
			break;
		}
	}
	ReleaseMutex(hMutex);

	sendData(hClientSock, RegisterDone.buf, sizeof(RegisterDone), 0);

	mysql_stmt_free_result(stmt);
	return;
}