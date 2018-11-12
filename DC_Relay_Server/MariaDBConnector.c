#include "RelayServer.h"

#pragma comment (lib, "libmariadb.lib")
#pragma comment (lib, "mariadbclient.lib")

void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo) {
	mysql_init(sqlHandle);
	mysql_real_connect(sqlHandle, serverInfo.srvAddr, serverInfo.user,
		serverInfo.pass, serverInfo.dbase, serverInfo.srvPort, NULL, 0);
	
	if (mysql_errno(sqlHandle)) {
		printDebugMsg(3, DC_ERRORLEVEL, "MariaDB Error!! Program Exit");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_error(sqlHandle));
		system("pause");
		exit(1);//exit with error
	}
	else {
		printDebugMsg(1, DC_ERRORLEVEL, "Successfully Connected to MariaDB Server.");
	}
}

void sqlPrepareAndExecute(MYSQL *sqlHandle, MYSQL_STMT *stmt, const char *query,  MYSQL_BIND *query_bind , MYSQL_BIND *result_bind){

	if (mysql_stmt_prepare(stmt, query, (unsigned long)strlen(query))) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (mysql_stmt_bind_param(stmt, query_bind)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Binding Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (mysql_stmt_execute(stmt)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: SQL Prepared Statement Execution fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	if (result_bind == NULL) return; //if this query has no resultset => end function

	if (mysql_stmt_bind_result(stmt, result_bind)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: Result binding Fail!!!");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}
	return;

}