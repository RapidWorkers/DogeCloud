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
	@author 멍멍아야옹해봐
	@brief RelayServer 유저 세션 처리 함수 모음
*/

#include "RelayServer.h"

/**
	@fn void procLoginStart(SOCKET hClientSock)
	@brief 로그인 처리 시작
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procLoginStart(SOCKET hClientSock) {
	//패킷 선언 및 초기화
	cs_LoginStart LoginStart;
	sc_LoginStartResp LoginStartResp;
	memset(&LoginStart, 0, sizeof(cs_LoginStart));
	memset(&LoginStartResp, 0, sizeof(sc_LoginStartResp));

	//클라이언트로부터 남은 데이터 수신
	recvData(hClientSock, LoginStart.buf + 4, sizeof(cs_LoginStart) - 4, 0);

	//호스트 특정 엔디언으로 변경
	LoginStart.Data.opCode = OP_CS_LOGINSTART;
	LoginStart.Data.dataLen = ntohl(LoginStart.Data.dataLen);

	//패킷 설정
	LoginStartResp.Data.opCode = htonl(OP_SC_LOGINSTARTRESP);
	LoginStartResp.Data.dataLen = htonl(sizeof(sc_LoginStartResp) - 8);
	LoginStartResp.Data.statusCode = 1;

	//응답패킷 전송
	sendData(hClientSock, LoginStartResp.buf, sizeof(LoginStartResp), 0);
	
	//실제 로그인 처리기로 넘김
	doLogin(hClientSock);
	return;
}

/**
	@fn void doLogin(SOCKET hClientSock)
	@brief 실제 로그인 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void doLogin(SOCKET hClientSock) {
	//패킷 선언 및 초기화
	cs_LoginAccountData LoginAccountData;
	sc_LoginDoneResp LoginDoneResp;
	memset(&LoginAccountData, 0, sizeof(cs_LoginAccountData));
	memset(&LoginDoneResp, 0, sizeof(sc_LoginDoneResp));

	char *query = "SELECT count(id), id FROM accounts WHERE username = ? and pwd = ?;";

	//클라이언트에서 남은 데이터 받아옴
	recvData(hClientSock, LoginAccountData.buf, sizeof(LoginAccountData), 0);

	//호스트 특정 엔디안으로 변경
	LoginAccountData.Data.opCode = ntohl(LoginAccountData.Data.opCode);
	LoginAccountData.Data.dataLen = ntohl(LoginAccountData.Data.dataLen);

	//패킷 설정
	LoginDoneResp.Data.opCode = htonl(OP_SC_LOGINDONERESP);
	LoginDoneResp.Data.dataLen = htonl(sizeof(sc_LoginDoneResp) - 8);

	//패스워드 해쉬로 변환
	unsigned char hashedPassword[32];
	SHA256_Text(LoginAccountData.Data.Password, hashedPassword);
	unsigned char hashedPasswordText[65] = { 0, };
	for (int i = 0; i < 32; i++) {
		sprintf_s(hashedPasswordText + (2 * i), 3, "%02x", hashedPassword[i]);
	}

	//Prepared Statement를 위해 준비
	MYSQL_STMT *stmt = NULL;
	MYSQL_BIND bind[2];
	memset(bind, 0, sizeof(bind)); //바인드 초기화

	//쿼리 바인더 준비
	bind[0].buffer_type = MYSQL_TYPE_STRING;
	bind[0].buffer = LoginAccountData.Data.Username;
	bind[0].buffer_length = (unsigned long)strlen(LoginAccountData.Data.Username);

	bind[1].buffer_type = MYSQL_TYPE_STRING;
	bind[1].buffer = hashedPasswordText;
	bind[1].buffer_length = 64;

	//결과 바인더 준비
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

	//MySQL Prepared Statement 초기화
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL 실행
	sqlPrepareAndExecute(&sqlHandle, stmt, query, bind, bind_result);

	//결과물 가져오기
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (loginFlag == 1) {//로그인 성공시
		GenerateSessionKey(LoginDoneResp.Data.sessionKey);

		WaitForSingleObject(hMutex, INFINITE);//to protect global var access
		for (int i = 0; i < clientCount; i++) {
			if (hClientSock == hClientSocks[i]) {
				memcpy(sessionList[i].sessionKey, LoginDoneResp.Data.sessionKey, 32); //생성된 세션키 전역변수에 저장
				sessionList[i].userUID = userUID;//User 번호 저장
				sessionList[i].currentStatus = 1;//현재 로그인 상태 저장
				break;
			}
		}
		ReleaseMutex(hMutex);

		LoginDoneResp.Data.statusCode = 1;
	}
	else {//실패시
		LoginDoneResp.Data.statusCode = 0;
	}

	//클라이언트에 데이터 전송
	sendData(hClientSock, LoginDoneResp.buf, sizeof(LoginDoneResp), 0);
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "Sent LoginDoneResp to Client");

	//mysql 정리
	mysql_stmt_close(stmt);

	return;
}

/**
	@fn void procLogout(SOCKET hClientSock)
	@brief 로그아웃 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procLogout(SOCKET hClientSock) {
	//패킷 선언 및 초기화
	cs_LogoutStart LogoutStart;
	sc_LogoutDone LogoutDone;
	memset(&LogoutStart, 0, sizeof(cs_LogoutStart));
	memset(&LogoutDone, 0, sizeof(sc_LogoutDone));

	//클라이언트로부터 나머지 데이터 받아옴
	recvData(hClientSock, LogoutStart.buf + 4, sizeof(LogoutStart) - 4, 0);

	//패킷 설정
	LogoutDone.Data.opCode = htonl(OP_SC_LOGOUTDONE);
	LogoutDone.Data.dataLen = htonl(1);
	LogoutDone.Data.statusCode = 1;

	//응답 패킷 전송
	sendData(hClientSock, LogoutDone.buf, sizeof(LogoutDone), 0);

	//쓰레드 종료시 자동으로 전역변수는 처리되므로 여기서 처리할 필요 없다.
}

/**
	@fn void procRegisterStart(SOCKET hClientSock)
	@brief 회원가입 처리 시작
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procRegisterStart(SOCKET hClientSock) {
	//패킷 선언 및 초기화
	cs_RegisterStart RegisterStart;
	sc_RegisterStartResp RegisterStartResp;
	memset(&RegisterStart, 0, sizeof(RegisterStart));
	memset(&RegisterStartResp, 0, sizeof(RegisterStartResp));

	//패킷 설정
	RegisterStart.Data.opCode = OP_CS_LOGINACCOUNTDATA;
	RegisterStart.Data.dataLen = sizeof(cs_LoginAccountData) - 8;

	//남은 데이터 받음
	recvData(hClientSock, RegisterStart.buf + 4, sizeof(RegisterStart) - 4, 0);

	//패킷 설정
	RegisterStartResp.Data.opCode = htonl(OP_SC_REGISTERSTARTRESP);
	RegisterStartResp.Data.dataLen = htonl(sizeof(RegisterStartResp) - 8);
	RegisterStartResp.Data.statusCode = 1;

	//응답패킷 전송
	sendData(hClientSock, RegisterStartResp.buf, sizeof(RegisterStartResp), 0);

	//실제 처리 함수로 넘김
	doRegister(hClientSock);

	return;
}

/**
	@fn void doRegister(SOCKET hClientSock)
	@brief 실제 회원가입 처리
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void doRegister(SOCKET hClientSock) {
	//패킷 선언 및 초기화
	cs_RegisterAccountData RegisterAccountData;
	sc_RegisterDone RegisterDone;
	memset(&RegisterAccountData, 0, sizeof(cs_RegisterAccountData));
	memset(&RegisterDone, 0, sizeof(sc_RegisterDone));

	//클라이언트에서 데이터 받아옴
	recvData(hClientSock, RegisterAccountData.buf, sizeof(RegisterAccountData), 0);

	char* unameDupChkQuery = "SELECT count(id) FROM accounts WHERE username = ?;";
	char* insertQuery = "INSERT into accounts VALUES(NULL, ?, ?, ?);";
	char* getUIDQuery = "SELECT id FROM accounts WHERE username = ?;";

	//Prepared Statement를 위한 변수 준비
	MYSQL_STMT *stmt = NULL;
	MYSQL_BIND query1_bind[1];
	MYSQL_BIND query2_bind[3];
	memset(query1_bind, 0, sizeof(query1_bind)); //init bind
	memset(query2_bind, 0, sizeof(query2_bind));

	//쿼리 바인더 설정
	query1_bind[0].buffer_type = MYSQL_TYPE_STRING;
	query1_bind[0].buffer = RegisterAccountData.Data.Username;
	query1_bind[0].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.Username);
	//bind[0].is_null = is_null;
	//bind[0].length = &usrNameLen; //why this is needed?

	//결과 바인더 설정
	int duplicateFlag = 0;
	MYSQL_BIND bind1_result;
	memset(&bind1_result, 0, sizeof(bind1_result)); //init bind
	bind1_result.buffer_type = MYSQL_TYPE_LONG;
	bind1_result.buffer = &duplicateFlag;
	bind1_result.buffer_length = 4;

	//MySQL Prepared Statement 준비
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL 실행
	sqlPrepareAndExecute(&sqlHandle, stmt, unameDupChkQuery, query1_bind, &bind1_result);

	//결과 받아오기
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (duplicateFlag) {//만약 중복이라면
		//패킷 설정
		RegisterDone.Data.opCode = htonl(OP_SC_REGISTERDONE);
		RegisterDone.Data.dataLen = htonl(sizeof(RegisterDone) - 8);
		RegisterDone.Data.statusCode = 1; //중복 코드 설정
		//패킷 전송
		sendData(hClientSock, RegisterDone.buf, sizeof(RegisterDone), 0);
		mysql_stmt_free_result(stmt);//MySQL 정리
		return;
	}

	mysql_stmt_free_result(stmt);//MySQL 정리

	//패스워드 해쉬 변환
	unsigned char hashedPassword[32];
	SHA256_Text(RegisterAccountData.Data.Password, hashedPassword);
	unsigned char hashedPasswordText[65] = { 0, };
	for (int i = 0; i < 32; i++) {
		sprintf_s(hashedPasswordText + (2 * i), 3, "%02x", hashedPassword[i]);
	}

	//쿼리 바인더 설정
	query2_bind[0].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[0].buffer = RegisterAccountData.Data.Username;
	query2_bind[0].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.Username);

	query2_bind[1].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[1].buffer = hashedPasswordText;
	query2_bind[1].buffer_length = 64;
	
	query2_bind[2].buffer_type = MYSQL_TYPE_STRING;
	query2_bind[2].buffer = RegisterAccountData.Data.email;
	query2_bind[2].buffer_length = (unsigned long)strlen(RegisterAccountData.Data.email);

	//Prepared Statement 준비 => 회원가입 쿼리
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL 실행 => 회원가입 처리 완료
	sqlPrepareAndExecute(&sqlHandle, stmt, insertQuery, query2_bind, NULL);
	mysql_stmt_free_result(stmt);//MySQL 정리

	//Prepared Statement 준비 => UID 가져오기
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//결과 바인더 준비
	int userUID = 0;
	MYSQL_BIND bind_result3;
	memset(&bind_result3, 0, sizeof(bind_result3)); //init bind
	bind_result3.buffer_type = MYSQL_TYPE_LONG;
	bind_result3.buffer = &userUID;
	bind_result3.buffer_length = 4;

	//SQL 실행
	sqlPrepareAndExecute(&sqlHandle, stmt, getUIDQuery, query1_bind, &bind_result3);

	//결과 가져오기
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//패킷 설정
	RegisterDone.Data.opCode = htonl(OP_SC_REGISTERDONE);
	RegisterDone.Data.dataLen = htonl(sizeof(RegisterDone) - 8);
	RegisterDone.Data.statusCode = 2;//회원가입 성공

	//세션키 생성
	GenerateSessionKey(RegisterDone.Data.sessionKey);
	char tmp[65] = { 0, };

	//로그인 처리
	WaitForSingleObject(hMutex, INFINITE);//to protect global var access
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			memcpy(sessionList[i].sessionKey, RegisterDone.Data.sessionKey, 32); //생성된 세션키 전역변수에 저장
			sessionList[i].userUID = userUID;//User 번호 저장
			sessionList[i].currentStatus = 1;//현재 로그인 상태 저장
			break;
		}
	}
	ReleaseMutex(hMutex);

	//패킷 전송
	sendData(hClientSock, RegisterDone.buf, sizeof(RegisterDone), 0);

	//MySQL 정리
	mysql_stmt_free_result(stmt);
	return;
}