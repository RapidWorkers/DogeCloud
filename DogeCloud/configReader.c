#include "DogeCloud.h"

void checkDogeConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./DogeConfig.ini", "r")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "DogeConfig 파일이 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 생성 해 주세요.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);//exit with error
	}
	fclose(fp);
	return;
}

void readRelayServerPath(SOCKADDR_IN *RelayServAddr) {
	checkDogeConfig();
	if (RelayServAddr == NULL) return;

	//init with zero
	memset(RelayServAddr, 0, sizeof(*RelayServAddr));

	//init addr
	char tmpAddr[16] = { 0, };
	RelayServAddr->sin_family = AF_INET;
	GetPrivateProfileString("RelayServer", "ip", "127.0.0.1", tmpAddr, 15, "./DogeConfig.ini");
	inet_pton(AF_INET, tmpAddr, &RelayServAddr->sin_addr.s_addr);

	//init port
	RelayServAddr->sin_port = htons(GetPrivateProfileInt("RelayServer", "port", 15384, "./DogeConfig.ini"));
	return;
}