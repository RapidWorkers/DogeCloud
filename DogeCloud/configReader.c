#include "DogeCloud.h"

void checkRelayConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./DogeConfig.ini", "r")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "DogeConfig ������ �����ϴ�.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "������ ���� �� �ּ���.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);//exit with error
	}
	fclose(fp);
	return;
}