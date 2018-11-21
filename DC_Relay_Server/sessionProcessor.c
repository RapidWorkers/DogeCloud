/*
Copyright (C) 2018 S.H.Kim (soohyunkim@kw.ac.kr)
Copyright (C) 2018 K.J Choi (chlrhkdwls99@naver.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/**
	@file sessionProcessor.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief RelayServer ���� ���� ó�� �Լ� ����
*/

#include "RelayServer.h"

/**
	@fn void procLoginStart(SOCKET hClientSock)
	@brief �α��� ó�� ����
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procLoginStart(SOCKET hClientSock) {
	//��Ŷ ���� �� �ʱ�ȭ
	cs_LoginStart LoginStart;
	sc_LoginStartResp LoginStartResp;
	memset(&LoginStart, 0, sizeof(cs_LoginStart));
	memset(&LoginStartResp, 0, sizeof(sc_LoginStartResp));

	//Ŭ���̾�Ʈ�κ��� ���� ������ ����
	recvData(hClientSock, LoginStart.buf + 4, sizeof(cs_LoginStart) - 4, 0);

	//ȣ��Ʈ Ư�� ��������� ����
	LoginStart.Data.opCode = OP_CS_LOGINSTART;
	LoginStart.Data.dataLen = ntohl(LoginStart.Data.dataLen);

	//��Ŷ ����
	LoginStartResp.Data.opCode = htonl(OP_SC_LOGINSTARTRESP);
	LoginStartResp.Data.dataLen = htonl(sizeof(sc_LoginStartResp) - 8);
	LoginStartResp.Data.statusCode = 1;

	//������Ŷ ����
	sendData(hClientSock, LoginStartResp.buf, sizeof(LoginStartResp), 0);
	
	//���� �α��� ó����� �ѱ�
	doLogin(hClientSock);
	return;
}

/**
	@fn void doLogin(SOCKET hClientSock)
	@brief ���� �α��� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void doLogin(SOCKET hClientSock) {
	//��Ŷ ���� �� �ʱ�ȭ
	cs_LoginAccountData LoginAccountData;
	sc_LoginDoneResp LoginDoneResp;
	memset(&LoginAccountData, 0, sizeof(cs_LoginAccountData));
	memset(&LoginDoneResp, 0, sizeof(sc_LoginDoneResp));

	char *query = "SELECT count(id), id FROM accounts WHERE username = ? and pwd = ?;";

	//Ŭ���̾�Ʈ���� ���� ������ �޾ƿ�
	recvData(hClientSock, LoginAccountData.buf, sizeof(LoginAccountData), 0);

	//ȣ��Ʈ Ư�� ��������� ����
	LoginAccountData.Data.opCode = ntohl(LoginAccountData.Data.opCode);
	LoginAccountData.Data.dataLen = ntohl(LoginAccountData.Data.dataLen);

	//��Ŷ ����
	LoginDoneResp.Data.opCode = htonl(OP_SC_LOGINDONERESP);
	LoginDoneResp.Data.dataLen = htonl(sizeof(sc_LoginDoneResp) - 8);

	//�н����� �ؽ��� ��ȯ
	unsigned char hashedPassword[32];
	SHA256_Text(LoginAccountData.Data.Password, hashedPassword);
	unsigned char hashedPasswordText[65] = { 0, };
	for (int i = 0; i < 32; i++) {
		sprintf_s(hashedPasswordText + (2 * i), 3, "%02x", hashedPassword[i]);
	}

	//Prepared Statement�� ���� �غ�
	MYSQL_STMT *stmt = NULL;
	MYSQL_BIND bind[2];
	memset(bind, 0, sizeof(bind)); //���ε� �ʱ�ȭ

	//���� ���δ� �غ�
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = LoginAccountData.Data.Username;
	bind[0].buffer_length = (unsigned long)strlen(LoginAccountData.Data.Username);

	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = hashedPasswordText;
	bind[1].buffer_length = 64;

	//��� ���δ� �غ�
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

	//MySQL Prepared Statement �ʱ�ȭ
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL ����
	sqlPrepareAndExecute(&sqlHandle, stmt, query, bind, bind_result);

	//����� ��������
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (loginFlag == 1) {//�α��� ������
		GenerateSessionKey(LoginDoneResp.Data.sessionKey);

		WaitForSingleObject(hMutex, INFINITE);//to protect global var access
		for (int i = 0; i < clientCount; i++) {
			if (hClientSock == hClientSocks[i]) {
				memcpy(sessionList[i].sessionKey, LoginDoneResp.Data.sessionKey, 32); //������ ����Ű ���������� ����
				sessionList[i].userUID = userUID;//User ��ȣ ����
				sessionList[i].currentStatus = 1;//���� �α��� ���� ����
				break;
			}
		}
		ReleaseMutex(hMutex);

		LoginDoneResp.Data.statusCode = 1;
	}
	else {//���н�
		LoginDoneResp.Data.statusCode = 0;
	}

	//Ŭ���̾�Ʈ�� ������ ����
	sendData(hClientSock, LoginDoneResp.buf, sizeof(LoginDoneResp), 0);
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Sent LoginDoneResp to Client");

	//mysql ����
	mysql_stmt_close(stmt);

	return;
}

/**
	@fn void procLogout(SOCKET hClientSock)
	@brief �α׾ƿ� ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procLogout(SOCKET hClientSock) {
	//��Ŷ ���� �� �ʱ�ȭ
	cs_LogoutStart LogoutStart;
	sc_LogoutDone LogoutDone;
	memset(&LogoutStart, 0, sizeof(cs_LogoutStart));
	memset(&LogoutDone, 0, sizeof(sc_LogoutDone));

	//Ŭ���̾�Ʈ�κ��� ������ ������ �޾ƿ�
	recvData(hClientSock, LogoutStart.buf + 4, sizeof(LogoutStart) - 4, 0);

	//��Ŷ ����
	LogoutDone.Data.opCode = htonl(OP_SC_LOGOUTDONE);
	LogoutDone.Data.dataLen = htonl(1);
	LogoutDone.Data.statusCode = 1;

	//���� ��Ŷ ����
	sendData(hClientSock, LogoutDone.buf, sizeof(LogoutDone), 0);

	//������ ����� �ڵ����� ���������� ó���ǹǷ� ���⼭ ó���� �ʿ� ����.
}

/**
	@fn void procRegisterStart(SOCKET hClientSock)
	@brief ȸ������ ó�� ����
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procRegisterStart(SOCKET hClientSock) {
	//��Ŷ ���� �� �ʱ�ȭ
	cs_RegisterStart RegisterStart;
	sc_RegisterStartResp RegisterStartResp;
	memset(&RegisterStart, 0, sizeof(RegisterStart));
	memset(&RegisterStartResp, 0, sizeof(RegisterStartResp));

	//��Ŷ ����
	RegisterStart.Data.opCode = OP_CS_LOGINACCOUNTDATA;
	RegisterStart.Data.dataLen = sizeof(cs_LoginAccountData) - 8;

	//���� ������ ����
	recvData(hClientSock, RegisterStart.buf + 4, sizeof(RegisterStart) - 4, 0);

	//��Ŷ ����
	RegisterStartResp.Data.opCode = htonl(OP_SC_REGISTERSTARTRESP);
	RegisterStartResp.Data.dataLen = htonl(sizeof(RegisterStartResp) - 8);
	RegisterStartResp.Data.statusCode = 1;

	//������Ŷ ����
	sendData(hClientSock, RegisterStartResp.buf, sizeof(RegisterStartResp), 0);

	//���� ó�� �Լ��� �ѱ�
	doRegister(hClientSock);

	return;
}

/**
	@fn void doRegister(SOCKET hClientSock)
	@brief ���� ȸ������ ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void doRegister(SOCKET hClientSock) {
	//��Ŷ ���� �� �ʱ�ȭ
	cs_RegisterAccountData RegisterAccountData;
	sc_RegisterDone RegisterDone;
	memset(&RegisterAccountData, 0, sizeof(cs_RegisterAccountData));
	memset(&RegisterDone, 0, sizeof(sc_RegisterDone));

	//Ŭ���̾�Ʈ���� ������ �޾ƿ�
	recvData(hClientSock, RegisterAccountData.buf, sizeof(RegisterAccountData), 0);

	char* unameDupChkQuery = "SELECT count(id) FROM accounts WHERE username = ?;";
	char* insertQuery = "INSERT into accounts VALUES(NULL, ?, ?, ?);";
	char* getUIDQuery = "SELECT id FROM accounts WHERE username = ?;";

	//Prepared Statement�� ���� ���� �غ�
	MYSQL_STMT *stmt = NULL;
	MYSQL_BIND query1_bind[1];
	MYSQL_BIND query2_bind[3];
	memset(query1_bind, 0, sizeof(query1_bind)); //init bind
	memset(query2_bind, 0, sizeof(query2_bind));

	//���� ���δ� ����
	query1_bind[0].buffer_type = MYSQL_TYPE_STRING;
	query1_bind[0].buffer = RegisterAccountData.Data.Username;
	query1_bind[0].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.Username);
	//bind[0].is_null = is_null;
	//bind[0].length = &usrNameLen; //why this is needed?

	//��� ���δ� ����
	int duplicateFlag = 0;
	MYSQL_BIND bind1_result;
	memset(&bind1_result, 0, sizeof(bind1_result)); //init bind
	bind1_result.buffer_type = MYSQL_TYPE_LONG;
	bind1_result.buffer = &duplicateFlag;
	bind1_result.buffer_length = 4;

	//MySQL Prepared Statement �غ�
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL ����
	sqlPrepareAndExecute(&sqlHandle, stmt, unameDupChkQuery, query1_bind, &bind1_result);

	//��� �޾ƿ���
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (duplicateFlag) {//���� �ߺ��̶��
		//��Ŷ ����
		RegisterDone.Data.opCode = htonl(OP_SC_REGISTERDONE);
		RegisterDone.Data.dataLen = htonl(sizeof(RegisterDone) - 8);
		RegisterDone.Data.statusCode = 1; //�ߺ� �ڵ� ����
		//��Ŷ ����
		sendData(hClientSock, RegisterDone.buf, sizeof(RegisterDone), 0);
		mysql_stmt_free_result(stmt);//MySQL ����
		return;
	}

	mysql_stmt_free_result(stmt);//MySQL ����

	//�н����� �ؽ� ��ȯ
	unsigned char hashedPassword[32];
	SHA256_Text(RegisterAccountData.Data.Password, hashedPassword);
	unsigned char hashedPasswordText[65] = { 0, };
	for (int i = 0; i < 32; i++) {
		sprintf_s(hashedPasswordText + (2 * i), 3, "%02x", hashedPassword[i]);
	}

	//���� ���δ� ����
	query2_bind[0].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[0].buffer = RegisterAccountData.Data.Username;
	query2_bind[0].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.Username);

	query2_bind[1].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[1].buffer = hashedPasswordText;
	query2_bind[1].buffer_length = 64;
	
	query2_bind[2].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[2].buffer = RegisterAccountData.Data.email;
	query2_bind[2].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.email);

	//Prepared Statement �غ� => ȸ������ ����
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL ���� => ȸ������ ó�� �Ϸ�
	sqlPrepareAndExecute(&sqlHandle, stmt, insertQuery, query2_bind, NULL);
	mysql_stmt_free_result(stmt);//MySQL ����

	//Prepared Statement �غ� => UID ��������
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//��� ���δ� �غ�
	int userUID = 0;
	MYSQL_BIND bind_result3;
	memset(&bind_result3, 0, sizeof(bind_result3)); //init bind
	bind_result3.buffer_type = MYSQL_TYPE_LONG;
	bind_result3.buffer = &userUID;
	bind_result3.buffer_length = 4;

	//SQL ����
	sqlPrepareAndExecute(&sqlHandle, stmt, getUIDQuery, query1_bind, &bind_result3);

	//��� ��������
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//��Ŷ ����
	RegisterDone.Data.opCode = htonl(OP_SC_REGISTERDONE);
	RegisterDone.Data.dataLen = htonl(sizeof(RegisterDone) - 8);
	RegisterDone.Data.statusCode = 2;//ȸ������ ����

	//����Ű ����
	GenerateSessionKey(RegisterDone.Data.sessionKey);
	char tmp[65] = { 0, };

	//�α��� ó��
	WaitForSingleObject(hMutex, INFINITE);//to protect global var access
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			memcpy(sessionList[i].sessionKey, RegisterDone.Data.sessionKey, 32); //������ ����Ű ���������� ����
			sessionList[i].userUID = userUID;//User ��ȣ ����
			sessionList[i].currentStatus = 1;//���� �α��� ���� ����
			break;
		}
	}
	ReleaseMutex(hMutex);

	//��Ŷ ����
	sendData(hClientSock, RegisterDone.buf, sizeof(RegisterDone), 0);

	//MySQL ����
	mysql_stmt_free_result(stmt);
	return;
}