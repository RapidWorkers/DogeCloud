#include "DogeCloud.h"

void checkRelayConfig() {
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