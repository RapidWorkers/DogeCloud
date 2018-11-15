#include "DogeCloud.h"

void openFileServer(char *fileServerAddr, unsigned long fileServerPort) {

	SOCKADDR_IN fileSrvConAddr;
	SOCKET hFileSrvSock;
	memset(&fileSrvConAddr, 0, sizeof(fileSrvConAddr));


	fileSrvConAddr.sin_family = AF_INET;
	fileSrvConAddr.sin_port = htons(fileServerPort);
	inet_pton(AF_INET, fileServerAddr, &fileSrvConAddr.sin_addr.s_addr);

	hFileSrvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hFileSrvSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "Invalid File Server Socket");
		system("pause");
		return;
	}

	int err = (connect(hFileSrvSock, (SOCKADDR*)&fileSrvConAddr, sizeof(fileSrvConAddr)) == SOCKET_ERROR);
	if (err) //생성된 소켓으로 서버에 연결
	{
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FS Connection Failled: %d", WSAGetLastError());
		system("pause");
		return;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "성공적으로 파일서버에 연결되었습니다.\n");
	Sleep(500);
	system("pause");

	doFileManage(hFileSrvSock);

	if (closesocket(hFileSrvSock)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일서버 소켓 종료 실패: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
		return;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "아이피: %s 포트: %ul", fileServerAddr, fileServerPort);
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "개발중");
	system("pause");
	return;
}

void doFileManage(SOCKET hFileSrvSock) {
	
}

void moveFileListPage();//move filelist page
void showFileList();//show filelist
void doFileJob();//up or down