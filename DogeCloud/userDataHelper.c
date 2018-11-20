#include "DogeCloud.h"

void manageContacts(SOCKET hSocket) {
	system("cls");

	char originalHash[32];
	FILE *fp;

	if (fopen_s(&fp, "./myinfoClient.db", "r")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	getFileHash(fp, originalHash);//나중에 변경 확인을 위해 먼저 파일 해쉬를 저장함

	fclose(fp); 

	int maxpage = 2;
	int page = 1;//페이지 초기값
	while (1) {
		system("cls");
		printf_s("\n******************************************************************************************");
		printf_s("\n%4s %16s %30s %15s %15s %15s %15s", "순번", "이름", "이메일", "연락처1", "연락처2", "연락처3", "연락처4");
		printf_s("\n%4d %16s %30s %15s %15s %15s %15s", 1, "홍길동일", "myemail@myemailtest.com", "010-0000-0000", "010-0000-0000", "010-0000-0000", "010-0000-0000");
		printf_s("\t\t\t 페이지 %d / %d", page, maxpage);
		printf_s("\n******************************************************************************************\n");
		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 추가");
		printf_s("\n\t2. 수정");
		printf_s("\n\t3. 이전 페이지");
		printf_s("\n\t4. 다음 페이지");
		printf_s("\n\t5. 변경사항 서버에 저장 및 나가기");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");

		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1:
			addContacts();
			break;
		case 2:
			modifyContacts();
			break;
		case 3:
			if (maxpage > page) page++;
			break;
		case 4:
			if (page > 1) page--;
			break;
		case 5:
			uploadPersonalDBFile(hSocket, originalHash);
			return;
			break;
		default: //유효하지 않은 입력
			printDebugMsg(DC_WARN, DC_ERRORLEVEL, "올바르지 않은 입력입니다.");
			Sleep(1000);
			break;
		}
	}
	return;
}

void manageMemo(SOCKET hSocket) {
	system("cls");

	char originalHash[32];
	FILE *fp;

	if (fopen_s(&fp, "./myinfoClient.db", "r")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	getFileHash(fp, originalHash);//나중에 변경 확인을 위해 먼저 파일 해쉬를 저장함

	fclose(fp);

	int maxpage = 2;
	int page = 1;//페이지 초기값
	while (1) {
		system("cls");
		printf_s("\n******************************************************************************************");
		printf_s("\n%4s %60s", "순번", "내용 일부 (한글 기준 최대 30자)");
		printf_s("\n%4d %60s", 1, "큰 여우가 작은 들을 넘어서 움직이고 있다. 그렇기에 나는 더  ");
		printf_s("\n%4d %60s", 2, "테스트 ");
		printf_s("\n\t\t\t 페이지 %d / %d", page, maxpage);
		printf_s("\n******************************************************************************************\n");
		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 추가");
		printf_s("\n\t2. 보기 및 수정");
		printf_s("\n\t3. 삭제");
		printf_s("\n\t4. 이전 페이지");
		printf_s("\n\t5. 다음 페이지");
		printf_s("\n\t6. 변경사항 서버에 저장 및 나가기");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");

		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1:
			addMemo();
			break;
		case 2:
			modifyMemo();
			break;
		case 3:
			deleteMemo();
			break;
		case 4:
			if (maxpage > page) page++;
			break;
		case 5:
			if (page > 1) page--;
			break;
		case 6:
			uploadPersonalDBFile(hSocket, originalHash);
			return;
			break;
		default: //유효하지 않은 입력
			printDebugMsg(DC_WARN, DC_ERRORLEVEL, "올바르지 않은 입력입니다.");
			Sleep(1000);
			break;
		}
	}
	return;
}

void manageFile(SOCKET hSocket) {
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	FileSrvConReq.Data.opCode = htonl(OP_CS_FILESRVCONNREQ);
	FileSrvConReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	sendData(hSocket, FileSrvConReq.buf, sizeof(cs_FileSrvConReq), 0);
	recvData(hSocket, FileSrvConResp.buf, sizeof(sc_FileSrvConResp), 0);

	//convert number to host specific
	FileSrvConResp.Data.opCode = ntohl(FileSrvConResp.Data.opCode);
	FileSrvConResp.Data.dataLen = ntohl(FileSrvConResp.Data.dataLen);
	
	if (!FileSrvConResp.Data.statusCode) {
		puts("파일서버 정보를 가져올 수 없습니다!");
		puts("파일 서버에 문제가 있거나 잠시 처리가 지연되는 중일 수 있습니다. 나중에 다시 시도해주세요.");
		system("pause");
		return;
	}

	openFileServer(FileSrvConResp.Data.fileSrvAddr, FileSrvConResp.Data.fileSrvPort, FileSrvConResp.Data.authKey);
	return;
}