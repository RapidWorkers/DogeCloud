#include "RelayServer.h"

void checkRelayConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./RelayServerConfig.ini", "r")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FATAL ERROR: RelayServerConfig.ini File Error");
		system("pause");
		exit(1);//exit with error
	}
	fclose(fp);
	return;
}

void readMySQLConfig(MYSQL_SERVER *serverInfo) {
	checkRelayConfig();

	if (serverInfo == NULL) return;
	GetPrivateProfileString("DatabaseConfig", "mariadb_addr", "localhost", serverInfo->srvAddr, 255, "./RelayServerConfig.ini");
	serverInfo->srvPort = GetPrivateProfileInt("DatabaseConfig", "mariadb_port", 3306, "./RelayServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_db", "DogeCloud", serverInfo->dbase, 255, "./RelayServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_user", "root", serverInfo->user, 255, "./RelayServerConfig.ini");
	GetPrivateProfileString("DatabaseConfig", "mariadb_pwd", "", serverInfo->pass, 255, "./RelayServerConfig.ini");

	return;
}

void readFileServerPath(SOCKADDR_IN *FileServAddr) {
	checkRelayConfig();
	if (FileServAddr == NULL) return;

	//init with zero
	memset(FileServAddr, 0, sizeof(*FileServAddr));

	//init addr
	char tmpAddr[16] = { 0, };
	FileServAddr->sin_family = AF_INET;
	GetPrivateProfileString("FileServer", "ip", "127.0.0.1", tmpAddr, 15, "./RelayServerConfig.ini");
	inet_pton(AF_INET, tmpAddr, &FileServAddr->sin_addr.s_addr);

	//init port
	FileServAddr->sin_port = htons(GetPrivateProfileInt("FileServer", "port", 15332, "./RelayServerConfig.ini"));
	return;
}