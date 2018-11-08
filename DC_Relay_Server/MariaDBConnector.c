#include "RelayServer.h"

#pragma comment (lib, "libmariadb.lib")
#pragma comment (lib, "mariadbclient.lib")

void sqlInit(MYSQL *sqlHandle, MYSQL_SERVER serverInfo) {
	mysql_init(sqlHandle);
	mysql_real_connect(sqlHandle, serverInfo.srvAddr, serverInfo.user,
		serverInfo.pass, serverInfo.dbase, serverInfo.srvPort, NULL, 0);
	
	if (mysql_errno(sqlHandle)) {
		printDebugMsg(3, ERLEVEL, "MariaDB Error!! Program Exit");
		printf("MySQL Error: %s", mysql_error(sqlHandle));
		system("pause");
		exit(1);//exit with error
	}
	else {
		printDebugMsg(1, ERLEVEL, "Successfully Connected to MariaDB Server.");
	}
}