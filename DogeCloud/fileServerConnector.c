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
	if (err) //������ �������� ������ ����
	{
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FS Connection Failled: %d", WSAGetLastError());
		system("pause");
		return;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "���������� ���ϼ����� ����Ǿ����ϴ�.\n");
	Sleep(500);
	system("pause");

	doFileManage(hFileSrvSock);

	if (closesocket(hFileSrvSock)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���ϼ��� ���� ���� ����: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
		return;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "������: %s ��Ʈ: %ul", fileServerAddr, fileServerPort);
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "������");
	system("pause");
	return;
}

void doFileManage(SOCKET hFileSrvSock) {
	
}

void moveFileListPage();//move filelist page
void showFileList();//show filelist
void doFileJob();//up or down