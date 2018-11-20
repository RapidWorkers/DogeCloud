#include "DogeCloud.h"

void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey) {

	SOCKADDR_IN fileSrvConAddr;
	SOCKET hFileSrvSock;
	memset(&fileSrvConAddr, 0, sizeof(fileSrvConAddr));

	//패킷을 0으로 초기화한다.
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
	if (err) //생성된 소켓으로 서버에 연결
	{
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "FS Connection Failled: %d", WSAGetLastError());
		system("pause");
		return;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "성공적으로 파일서버에 연결되었습니다.");
	Sleep(500);
	system("pause");

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "파일서버 인증 시작.");
	memcpy(LoginFile.Data.UserFileServerAuthKey, authKey, 32);
	LoginFile.Data.opCode = htonl(OP_CF_LOGINFILE);
	LoginFile.Data.dataLen = htonl(sizeof(cf_LoginFile) - 8);

	sendData(hFileSrvSock, LoginFile.buf, sizeof(cf_LoginFile), 0);
	recvData(hFileSrvSock, LoginFileResp.buf, sizeof(fc_LoginFileResp), 0);

	//패킷 변환
	LoginFileResp.Data.opCode = ntohl(LoginFileResp.Data.opCode);
	LoginFileResp.Data.dataLen = ntohl(LoginFileResp.Data.dataLen);

	if (LoginFileResp.Data.statusCode == 0) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일서버 부정 접속 감지");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
		return;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "파일서버 인증 성공.");
	Sleep(500);

	doFileManage(hFileSrvSock);

	if (closesocket(hFileSrvSock)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일서버 소켓 종료 실패: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
		return;
	}
	return;
}

void doFileManage(SOCKET hFileSrvSock) {
	int currentPage = 1; //처음 페이지는 1페이지
	while (1) {

		int errorFlag = 0;

		system("cls");
		showFileList(hFileSrvSock, &errorFlag);

		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 업로드");
		printf_s("\n\t2. 다운로드");
		printf_s("\n\t3. 폴더 이동");
		printf_s("\n\t4. 이전 페이지");
		printf_s("\n\t5. 다음 페이지");
		printf_s("\n\t6. 나가기");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");

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
			return;//이 함수 종료시 계속되는 코드에서 소켓 종료됨
			break;
		default: //유효하지 않은 입력
			printDebugMsg(DC_WARN, DC_ERRORLEVEL, "올바르지 않은 입력입니다.");
			Sleep(1000);
			break;
		}

		if (errorFlag) {
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일서버 통신 오류");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일서버 연결을 종료합니다.");
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
	printf_s("이동할 디렉토리 입력(최대 254자) : ");
	scanf_s("%s", directory, 255);

	MoveDir.Data.opCode = htonl(OP_CF_MOVEDIR);
	MoveDir.Data.dataLen = htonl(sizeof(cf_MoveDir) - 8);
	strcpy_s(MoveDir.Data.moveDir, 255, directory);

	*errorFlag = 1;//일단 errorFlag = 1로 설정
	sendData(hFileSrvSock, MoveDir.buf, sizeof(cf_MoveDir), 0);
	recvData(hFileSrvSock, MoveDirResp.buf, sizeof(fc_MoveDirResp), 0);
	*errorFlag = 0;//위 함수들이 정상적으로 종료되면 계속 함수가 진행되므로 다시 0으로 설정

	MoveDirResp.Data.opCode = ntohl(MoveDirResp.Data.opCode);
	MoveDirResp.Data.dataLen = ntohl(MoveDirResp.Data.dataLen);

	if (!MoveDirResp.Data.statusCode) {//0을 반환시
		printDebugMsg(DC_WARN, DC_ERRORLEVEL, "존재하지 않는 디렉토리이거나 요청이 실패했습니다.");
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

	*errorFlag = 1;//일단 errorFlag = 1로 설정
	sendData(hFileSrvSock, ListPageMove.buf, sizeof(cf_ListPageMove), 0);
	recvData(hFileSrvSock, ListPageMoveResp.buf, sizeof(fc_ListPageMoveResp), 0);
	*errorFlag = 0;//위 함수들이 정상적으로 종료되면 계속 함수가 진행되므로 다시 0으로 설정

	ListPageMove.Data.opCode = ntohl(ListPageMove.Data.opCode);
	ListPageMove.Data.dataLen = ntohl(ListPageMove.Data.dataLen);

	if (!ListPageMoveResp.Data.statusCode) {//0을 반환시
		*errorFlag = 1;
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "요청이 실패했습니다.");
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

	*errorFlag = 1;//일단 errorFlag = 1로 설정
	sendData(hFileSrvSock, ListFile.buf, sizeof(cf_ListFile), 0);
	recvData(hFileSrvSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);
	*errorFlag = 0;//위 함수들이 정상적으로 종료되면 계속 함수가 진행되므로 다시 0으로 설정

	ListFileResp.Data.opCode = ntohl(ListFileResp.Data.opCode);
	ListFileResp.Data.dataLen = ntohl(ListFileResp.Data.dataLen);
	
	if (!ListFileResp.Data.statusCode) {
		*errorFlag = 1;//파일 목록을 불러오지 못했으므로
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일서버에서 파일 목록을 가져오지 못했습니다.");
		return;
	}

	printf_s("\n\t******************************");
	printf_s("\n\t\t현재 디렉토리 : %s", ListFileResp.Data.currentDir);
	if (!ListFileResp.Data.fileCount) {//파일이 없을 경우
		printf_s("\n\t 빈 디렉토리 입니다.");
	}
	else {//비어있지 않으면
		printf_s("\n\t순번 \t파일명 \t파일 타입");
		for (int i = 0; i < ListFileResp.Data.fileCount; i++) {
			printf_s("\n\t%d. %s ", i+1, ListFileResp.Data.fileName[i]);
			if (ListFileResp.Data.fileType[i] == 0)
				printf_s("파일");
			else if (ListFileResp.Data.fileType[i] == 1)
				printf_s("폴더");
		}
	}
	printf_s("\n\t\t페이지 %d / %d", ListFileResp.Data.currentPage, ListFileResp.Data.totalPage);
	printf_s("\n\t******************************\n");
}

void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag) {//up or down

}