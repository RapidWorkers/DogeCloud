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
	@author �۸۾ƾ߿��غ�
	@brief MariaDB(MySQL) ���� �Լ� ����
*/

#include "FileServer.h"

//dll �ε�
#pragma comment (lib, "libmariadb.lib")
#pragma comment (lib, "mariadbclient.lib")

/**
	@fn void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo)
	@brief SQL ���� ����
	@author �۸۾ƾ߿��غ�
	@param *sqlHandle MySQL Handle ������
	@param serverInfo MySQL ���� ���� ����ü
*/
void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo) {
	mysql_init(sqlHandle);//�ڵ� �ʱ�ȭ
	mysql_real_connect(sqlHandle, serverInfo.srvAddr, serverInfo.user,
		serverInfo.pass, serverInfo.dbase, serverInfo.srvPort, NULL, 0);//����
	
	if (mysql_errno(sqlHandle)) {//���� �߻���
		printDebugMsg(3, DC_ERRORLEVEL, "MariaDB Error!! Program Exit");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_error(sqlHandle));
		system("pause");
		exit(1);//exit with error
	}
	else {//���� ������
		printDebugMsg(1, DC_ERRORLEVEL, "Successfully Connected to MariaDB Server.");
	}
	return;
}

/**
	@fn void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query, MYSQL_BIND *query_bind, MYSQL_BIND *result_bind)
	@brief SQL Prepared Statement �غ� �� ���� ����
	@author �۸۾ƾ߿��غ�
	@param *sqlHandle MySQL Handle ������
	@param *stmt MySQL stmt ������
	@param *query ������ ������
	@param *query_bind ������ ���ε��� ���� ������
	@param *result_bind ������� ���ε��� ���� ������
*/
void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query,  MYSQL_BIND *query_bind , MYSQL_BIND *result_bind){

	if (mysql_stmt_prepare(stmt, query, (unsigned long)strlen(query))) {//�غ�
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (mysql_stmt_bind_param(stmt, query_bind)) {//������ ���ε�
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Binding Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (mysql_stmt_execute(stmt)) {//���� ����
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Execution fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (result_bind == NULL) return; //����� ���ε� �����Ͱ� null�̸� �׳� ����

	if (mysql_stmt_bind_result(stmt, result_bind)) {//����� ���ε�
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Result binding Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}
	return;

}