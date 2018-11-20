#include "DogeCloud.h"
#include <sqlite3.h>

void downloadPersonalDBFile(SOCKET hSocket) {
	cs_DownloadPersonalDBReq DownloadInfoReq;
	sc_DownloadPersonalDBResp DownloadInfoResp;
	memset(&DownloadInfoReq, 0, sizeof(cs_DownloadPersonalDBReq));
	memset(&DownloadInfoResp, 0, sizeof(sc_DownloadPersonalDBResp));

	DownloadInfoReq.Data.opCode = htonl(OP_CS_DOWNLOADUSERINFOREQ);
	DownloadInfoReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	sendData(hSocket, DownloadInfoReq.buf, sizeof(cs_DownloadPersonalDBReq), 0);
	//다운로드 모드 시작
	FILE *downFile;
	unsigned int toRead;

	if ((downFile = fopen("myinfoClient.db", "wb+")) == NULL) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 쓰기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	unsigned long fileSize;
	recvRaw(hSocket, &fileSize, 4, 0);
	fileSize = ntohl(fileSize);
	unsigned long left = fileSize;

	puts("\n유저 정보 다운로드 시작...");

	unsigned char dataBuffer[4096]; //4KiB

	while (1) {
		if (left < 4096U)
			toRead = left;
		else
			toRead = 4096U;

		if (!recvRaw(hSocket, dataBuffer, toRead, 0)) {
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "전송 실패");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
			system("pause");
			exit(1);
		}
		fwrite(dataBuffer, toRead, 1, downFile);
		left -= toRead;
		updateProgress(fileSize - left, fileSize);
		if (!left) break;//완료시 탈출
	}

	puts("\n수신 완료");
	//다운로드 모드 종료
	recvData(hSocket, DownloadInfoResp.buf, sizeof(sc_DownloadPersonalDBResp), 0);
	DownloadInfoResp.Data.opCode = ntohl(DownloadInfoResp.Data.opCode);
	DownloadInfoResp.Data.dataLen = ntohl(DownloadInfoResp.Data.dataLen);

	unsigned char fileHash[32];
	getFileHash(downFile, fileHash);

	if (!memcmp(fileHash, DownloadInfoResp.Data.hash, 32)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터 불일치");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	fclose(downFile);
	return;
}

void uploadPersonalDBFile(SOCKET hSocket, char* originalHash) {
	cs_PersonalDBEditDone PersonalDBEditDone;
	sc_PersonalDBEditDoneResp PersonalDBEditDoneResp;
	memset(&PersonalDBEditDone, 0, sizeof(cs_PersonalDBEditDone));
	memset(&PersonalDBEditDoneResp, 0, sizeof(sc_PersonalDBEditDoneResp));

	unsigned char fileHash[32];
	FILE *infoFile;

	if (fopen_s(&infoFile, "./myinfoClient.db", "rb")) {//해쉬를 구하기 위해서
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 읽기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	getFileHash(infoFile, fileHash);

	if (!memcmp(fileHash, originalHash, 32))//만약에 파일이 수정되지 않았다면
		return;//그냥 종료한다

	//수정된 파일이라면 계속 진행한다.

	PersonalDBEditDone.Data.opCode = htonl(OP_CS_PERSONALDBEDITDONE);
	PersonalDBEditDone.Data.dataLen = htonl(sizeof(cs_PersonalDBEditDone) - 8);
	memcpy(PersonalDBEditDone.Data.hash, fileHash, 32);

	int flag = 0;
	int count = 0;
	while (1) {
		puts("\n유저 정보 업로드 시작...");
		count++;
		sendData(hSocket, PersonalDBEditDone.buf, sizeof(cs_PersonalDBEditDone), 0);

		//업로드 모드 진입
		fseek(infoFile, 0, SEEK_END);
		unsigned int fileSize = ftell(infoFile);
		unsigned int left = fileSize;
		unsigned int toRead;
		fseek(infoFile, 0, SEEK_SET);

		//파일 사이즈 전송
		sendData(hSocket, &fileSize, 4, 0);

		unsigned char dataBuffer[4096]; //4KiB
		while (1) {
			if (left < 4096U)
				toRead = left;
			else
				toRead = 4096U;

			fread(dataBuffer, toRead, 1, infoFile);

			if (!sendRaw(hSocket, dataBuffer, toRead, 0)) {
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "전송 실패");
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
				system("pause");
				exit(1);
			}

			left -= toRead;
			updateProgress(fileSize - left, fileSize);
			if (!left) break;//완료시 탈출
		}

		//업로드 모드 끝
		recvData(hSocket, PersonalDBEditDoneResp.buf, sizeof(sc_PersonalDBEditDoneResp), 0);
		PersonalDBEditDoneResp.Data.opCode = ntohl(PersonalDBEditDoneResp.Data.opCode);
		PersonalDBEditDoneResp.Data.dataLen = ntohl(PersonalDBEditDoneResp.Data.dataLen);

		if (PersonalDBEditDoneResp.Data.statusCode)//성공시 탈출
			break;

		if (!flag && count == 3) {//3번 다 실패한 경우
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버에 파일을 저장할 수 없었습니다.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "myinfoClient.db 파일을 수동으로 백업해 주십시오.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "그 후 관리자에게 이메일로 전송해 주시면 수동으로 적용됩니다.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
			system("pause");
			exit(1);
		}
	}
	puts("\n유저 정보 업로드 완료");
	system("pause");
	return;
}

void addContacts() {

};

void modifyContacts() {

}

void deleteContacts() {

}

void addMemo() {
	char tmpFileName[65] = { 0, };
	unsigned char tmpRandomNum[16] = { 0, };
	unsigned char tmpHash[32] = { 0, };
	char cmd[255] = { 0, };
	FILE *tmpFile;
	sqlite3 *dbHandle;
	char *insertMemo = "";

	//임시로 사용할 파일이름 생성
	GenerateCSPRNG(tmpRandomNum, 15);
	SHA256_Text(tmpRandomNum, tmpHash);
	for (int i = 0; i < 32; i++)
		sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

	//임시파일 생성
	if (fopen_s(&tmpFile, tmpFileName, "wb+")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 쓰기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}
	fclose(tmpFile);//수정을 위해 외부 프로그램이 열 수 있도록 파일 해제


	sprintf_s(cmd, 255, "notepad %s", tmpFileName);
	puts("수정이 완료되면 저장하고 메모장을 닫아주세요.");
	system(cmd);

	if (fopen_s(&tmpFile, tmpFileName, "rb")) {//DB저장을 위해서 다시 파일을 연다
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 읽기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//TODO: db 처리

	//db 처리 완료

	fclose(tmpFile);//파일 해제
	remove(tmpFileName);//임시파일 삭제
	sqlite3_close(dbHandle);//db파일 닫기

	return;
}

void modifyMemo() {

}

void deleteMemo() {

}