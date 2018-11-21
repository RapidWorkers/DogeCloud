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
	@file MariaDBConnector.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief MariaDB(MySQL) 관련 함수 모음
*/

#include "FileServer.h"

//dll 로드
#pragma comment (lib, "libmariadb.lib")
#pragma comment (lib, "mariadbclient.lib")

/**
	@fn void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo)
	@brief SQL 서버 접속
	@author 멍멍아야옹해봐
	@param *sqlHandle MySQL Handle 포인터
	@param serverInfo MySQL 서버 정보 구조체
*/
void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo) {
	mysql_init(sqlHandle);//핸들 초기화
	mysql_real_connect(sqlHandle, serverInfo.srvAddr, serverInfo.user,
		serverInfo.pass, serverInfo.dbase, serverInfo.srvPort, NULL, 0);//접속
	
	if (mysql_errno(sqlHandle)) {//에러 발생시
		printDebugMsg(3, DC_ERRORLEVEL, "MariaDB Error!! Program Exit");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_error(sqlHandle));
		system("pause");
		exit(1);//exit with error
	}
	else {//에러 없으면
		printDebugMsg(1, DC_ERRORLEVEL, "Successfully Connected to MariaDB Server.");
	}
	return;
}

/**
	@fn void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind)
	@brief SQL Prepared Statement 준비 및 쿼리 실행
	@author 멍멍아야옹해봐
	@param *sqlHandle MySQL Handle 포인터
	@param *stmt MySQL stmt 포인터
	@param *query 실행할 쿼리문
	@param *query_bind 쿼리에 바인드할 것의 포인터
	@param *result_bind 결과물에 바인드할 것의 포인터
*/
void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query,  MYSQL_BIND *query_bind , MYSQL_BIND *result_bind){

	if (mysql_stmt_prepare(stmt, query, (unsigned long)strlen(query))) {//준비
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (mysql_stmt_bind_param(stmt, query_bind)) {//쿼리에 바인딩
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Binding Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (mysql_stmt_execute(stmt)) {//쿼리 실행
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Execution fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (result_bind == NULL) return; //결과물 바인딩 포인터가 null이면 그냥 종료

	if (mysql_stmt_bind_result(stmt, result_bind)) {//결과물 바인딩
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Result binding Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}
	return;

}