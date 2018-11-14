#include "FileServer.h"

void checkFileConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./FileServerConfig.ini", "r")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: FileServerConfig.ini File Error");
		system("pause");
		exit(1);//exit with error
	}
	fclose(fp);
	return;
}

void readMySQLConfig(MYSQL_SERVER *serverInfo) {
	checkFileConfig();

	if (serverInfo == NULL) return;
	GetPrivateProfileString("DatabaseConfig", "mariadb_addr", "localhost", serverInfo->srvAddr, 255, "./FileServerConfig.ini");
	serverInfo->srvPort = GetPrivateProfileInt("DatabaseConfig", "mariadb_port", 3306, "./FileServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_db", "DogeCloud", serverInfo->dbase, 255, "./FileServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_user", "root", serverInfo->user, 255, "./FileServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_pwd", "", serverInfo->pass, 255, "./FileServerConfig.ini");

	return;
}