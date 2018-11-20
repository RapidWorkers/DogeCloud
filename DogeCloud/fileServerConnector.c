#include "DogeCloud.h"

void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey) {

	SOCKADDR_IN fileSrvConAddr;
	SOCKET hFileSrvSock;
	memset(&fileSrvConAddr, 0, sizeof(fileSrvConAddr));

	//��Ŷ�� 0���� �ʱ�ȭ�Ѵ�.
	cf_LoginFile LoginFile;
	fc_LoginFileResp LoginFileResp;
	memset(&LoginFile, 0, sizeof(cf_LoginFile));
	memset(&LoginFileResp, 0, sizeof(fc_LoginFileResp));

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

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "���������� ���ϼ����� ����Ǿ����ϴ�.");
	Sleep(500);
	system("pause");

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "���ϼ��� ���� ����.");
	memcpy(LoginFile.Data.UserFileServerAuthKey, authKey, 32);
	LoginFile.Data.opCode = htonl(OP_CF_LOGINFILE);
	LoginFile.Data.dataLen = htonl(sizeof(cf_LoginFile) - 8);

	sendData(hFileSrvSock, LoginFile.buf, sizeof(cf_LoginFile), 0);
	recvData(hFileSrvSock, LoginFileResp.buf, sizeof(fc_LoginFileResp), 0);

	//��Ŷ ��ȯ
	LoginFileResp.Data.opCode = ntohl(LoginFileResp.Data.opCode);
	LoginFileResp.Data.dataLen = ntohl(LoginFileResp.Data.dataLen);

	if (LoginFileResp.Data.statusCode == 0) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���ϼ��� ���� ���� ����");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
		return;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "���ϼ��� ���� ����.");
	Sleep(500);

	doFileManage(hFileSrvSock);

	if (closesocket(hFileSrvSock)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���ϼ��� ���� ���� ����: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
		return;
	}
	return;
}

void doFileManage(SOCKET hFileSrvSock) {
	int currentPage = 1; //ó�� �������� 1������
	while (1) {

		int errorFlag = 0;

		system("cls");
		showFileList(hFileSrvSock, &errorFlag);

		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. ���ε�");
		printf_s("\n\t2. �ٿ�ε�");
		printf_s("\n\t3. ���� �̵�");
		printf_s("\n\t4. ���� ������");
		printf_s("\n\t5. ���� ������");
		printf_s("\n\t6. ������");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");

		int select;
		scanf_s("%d", &select);

		switch (select) {
		case 1:
			doFileJob(hFileSrvSock, 0, &errorFlag);
			break;
		case 2:
			doFileJob(hFileSrvSock, 1, &errorFlag);
			break;
		case 3:
			moveDir(hFileSrvSock, &errorFlag);
			break;
		case 4:
			moveFileListPage(hFileSrvSock, 0, &errorFlag);
			break;
		case 5:
			moveFileListPage(hFileSrvSock, 1, &errorFlag);
			break;
		case 6:
			return;//�� �Լ� ����� ��ӵǴ� �ڵ忡�� ���� �����
			break;
		default: //��ȿ���� ���� �Է�
			printDebugMsg(DC_WARN, DC_ERRORLEVEL, "�ùٸ��� ���� �Է��Դϴ�.");
			Sleep(1000);
			break;
		}

		if (errorFlag) {
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���ϼ��� ��� ����");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���ϼ��� ������ �����մϴ�.");
			system("pause");
			return;
		}

	}
}

void moveDir(SOCKET hFileSrvSock, int *errorFlag) {//move filelist page
	cf_MoveDir MoveDir;
	fc_MoveDirResp MoveDirResp;
	memset(&MoveDir, 0, sizeof(cf_MoveDir));
	memset(&MoveDirResp, 0, sizeof(fc_MoveDirResp));

	char directory[255];
	printf_s("�̵��� ���丮 �Է�(�ִ� 254��) : ");
	scanf_s("%s", directory, 255);

	MoveDir.Data.opCode = htonl(OP_CF_MOVEDIR);
	MoveDir.Data.dataLen = htonl(sizeof(cf_MoveDir) - 8);
	strcpy_s(MoveDir.Data.moveDir, 255, directory);

	*errorFlag = 1;//�ϴ� errorFlag = 1�� ����
	sendData(hFileSrvSock, MoveDir.buf, sizeof(cf_MoveDir), 0);
	recvData(hFileSrvSock, MoveDirResp.buf, sizeof(fc_MoveDirResp), 0);
	*errorFlag = 0;//�� �Լ����� ���������� ����Ǹ� ��� �Լ��� ����ǹǷ� �ٽ� 0���� ����

	MoveDirResp.Data.opCode = ntohl(MoveDirResp.Data.opCode);
	MoveDirResp.Data.dataLen = ntohl(MoveDirResp.Data.dataLen);

	if (!MoveDirResp.Data.statusCode) {//0�� ��ȯ��
		printDebugMsg(DC_WARN, DC_ERRORLEVEL, "�������� �ʴ� ���丮�̰ų� ��û�� �����߽��ϴ�.");
	}

	return;
}

void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag) {//move filelist page
	cf_ListPageMove ListPageMove;
	fc_ListPageMoveResp ListPageMoveResp;
	memset(&ListPageMove, 0, sizeof(cf_ListPageMove));
	memset(&ListPageMoveResp, 0, sizeof(fc_ListPageMoveResp));

	ListPageMove.Data.opCode = htonl(OP_CF_LISTPAGEMOVE);
	ListPageMove.Data.dataLen = htonl(sizeof(cf_ListPageMove) - 8);
	ListPageMove.Data.moveTo = type;

	*errorFlag = 1;//�ϴ� errorFlag = 1�� ����
	sendData(hFileSrvSock, ListPageMove.buf, sizeof(cf_ListPageMove), 0);
	recvData(hFileSrvSock, ListPageMoveResp.buf, sizeof(fc_ListPageMoveResp), 0);
	*errorFlag = 0;//�� �Լ����� ���������� ����Ǹ� ��� �Լ��� ����ǹǷ� �ٽ� 0���� ����

	ListPageMove.Data.opCode = ntohl(ListPageMove.Data.opCode);
	ListPageMove.Data.dataLen = ntohl(ListPageMove.Data.dataLen);

	if (!ListPageMoveResp.Data.statusCode) {//0�� ��ȯ��
		*errorFlag = 1;
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "��û�� �����߽��ϴ�.");
	}

	return;
}

void showFileList(SOCKET hFileSrvSock, int *errorFlag) {//show filelist

	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	ListFile.Data.opCode = htonl(OP_CF_LISTFILE);
	ListFile.Data.dataLen = htonl(sizeof(cf_ListFile) - 8);

	*errorFlag = 1;//�ϴ� errorFlag = 1�� ����
	sendData(hFileSrvSock, ListFile.buf, sizeof(cf_ListFile), 0);
	recvData(hFileSrvSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);
	*errorFlag = 0;//�� �Լ����� ���������� ����Ǹ� ��� �Լ��� ����ǹǷ� �ٽ� 0���� ����

	ListFileResp.Data.opCode = ntohl(ListFileResp.Data.opCode);
	ListFileResp.Data.dataLen = ntohl(ListFileResp.Data.dataLen);
	
	if (!ListFileResp.Data.statusCode) {
		*errorFlag = 1;//���� ����� �ҷ����� �������Ƿ�
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���ϼ������� ���� ����� �������� ���߽��ϴ�.");
		return;
	}

	printf_s("\n\t******************************");
	printf_s("\n\t\t���� ���丮 : %s", ListFileResp.Data.currentDir);
	if (!ListFileResp.Data.fileCount) {//������ ���� ���
		printf_s("\n\t �� ���丮 �Դϴ�.");
	}
	else {//������� ������
		printf_s("\n\t���� \t���ϸ� \t���� Ÿ��");
		for (int i = 0; i < ListFileResp.Data.fileCount; i++) {
			printf_s("\n\t%d. %s ", i+1, ListFileResp.Data.fileName[i]);
			if (ListFileResp.Data.fileType[i] == 0)
				printf_s("����");
			else if (ListFileResp.Data.fileType[i] == 1)
				printf_s("����");
		}
	}
	printf_s("\n\t\t������ %d / %d", ListFileResp.Data.currentPage, ListFileResp.Data.totalPage);
	printf_s("\n\t******************************\n");
}

void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag) {//up or down

}