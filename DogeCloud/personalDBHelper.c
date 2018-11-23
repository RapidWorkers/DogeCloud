/*
Copyright (C) 2018 S.H.Kim (soohyunkim@kw.ac.kr)
Copyright (C) 2018 K.J Choi (chlrhkdwls99@naver.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

/**
	@file personalDBHelper.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief 개인 DB 관련 함수 모음
*/

#include "DogeCloud.h"
#include <sqlite3.h>

/**
	@fn void downloadPersonalDBFile(SOCKET hSocket)
	@brief DogeCloud 개인 DB 파일 다운로드
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void downloadPersonalDBFile(SOCKET hSocket) {
	//패킷 공간 할당 및 초기화
	cs_DownloadPersonalDBReq DownloadInfoReq;
	sc_DownloadPersonalDBResp DownloadInfoResp;
	memset(&DownloadInfoReq, 0, sizeof(cs_DownloadPersonalDBReq));//0으로 초기화
	memset(&DownloadInfoResp, 0, sizeof(sc_DownloadPersonalDBResp));

	/** @brief 다운로드 받을 파일을 위한 구조체 */
	FILE *downFile;

	/** @brief 받아올 버퍼 크기 */
	unsigned int toWrite;

	/** @brief 받아올 파일 크기 */
	unsigned long fileSize;

	/** @brief 다운로드 버퍼 4KiB */
	unsigned char dataBuffer[4096];

	/** @brief 파일 해쉬값 저장용 */
	unsigned char fileHash[32];

	//패킷 설정
	DownloadInfoReq.Data.opCode = htonl(OP_CS_DOWNLOADUSERINFOREQ);
	DownloadInfoReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	//다운로드 요청 패킷 전송
	sendData(hSocket, DownloadInfoReq.buf, sizeof(cs_DownloadPersonalDBReq), 0);

	//다운로드 모드 시작
	if (fopen_s(&downFile, "myinfoClient.db", "wb+")) {//DB 파일 다운로드를 위해 파일 열기
		printDebugMsg(DC_ERROR, errorLevel, "파일을 쓰기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	recvData(hSocket, (char*)&fileSize, 4, 0);
	fileSize = ntohl(fileSize);//호스트 특정 인디안으로 변환

	/** @brief 남은 파일 크기 */
	unsigned long left = fileSize;

	puts("\n유저 정보 다운로드 시작...");

	while (1) {//남은 크기가 0이 될때 까지 반복
		if (left < 4096U)//4KB보다 작은만큼 남으면
			toWrite = left;//남은 만큼 받아옴
		else//아니면
			toWrite = 4096U;//4KiB만큼 받아옴

		if (!recvRaw(hSocket, dataBuffer, toWrite, 0)) {//서버로부터 다운로드
			printDebugMsg(DC_ERROR, errorLevel, "전송 실패");
			printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
			system("pause");
			exit(1);
		}

		fwrite(dataBuffer, toWrite, 1, downFile);//파일에 받아온 만큼 작성
		left -= toWrite;//받아온 만큼 뺀다
		updateProgress(fileSize - left, fileSize);//프로그레스 바 업데이트(생성)
		if (!left) break;//완료시 탈출
	}

	puts("\n수신 완료");
	//다운로드 모드 종료

	//다운로드 완료 패킷 가져옴
	recvData(hSocket, DownloadInfoResp.buf, sizeof(sc_DownloadPersonalDBResp), 0);

	//호스트 인디안으로 변환
	DownloadInfoResp.Data.opCode = ntohl(DownloadInfoResp.Data.opCode);
	DownloadInfoResp.Data.dataLen = ntohl(DownloadInfoResp.Data.dataLen);

	//다운로드 받은 파일의 해쉬 구하기
	getFileHash(downFile, fileHash);

	if (memcmp(fileHash, DownloadInfoResp.Data.hash, 32)) {//만약 해쉬값이 다르다면
		printDebugMsg(DC_ERROR, errorLevel, "데이터 불일치");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	fclose(downFile);//파일 닫기
	return;
}

/**
	@fn void uploadPersonalDBFile(SOCKET hSocket, char* originalHash)
	@brief DogeCloud 개인 DB 파일 업로드
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
	@param *originalHash 파일 해쉬값
*/
void uploadPersonalDBFile(SOCKET hSocket, char* originalHash) {
	//패킷 공간 할당 및 초기화
	cs_PersonalDBEditDone PersonalDBEditDone;
	sc_PersonalDBEditDoneResp PersonalDBEditDoneResp;
	memset(&PersonalDBEditDone, 0, sizeof(cs_PersonalDBEditDone));//0으로 초기화
	memset(&PersonalDBEditDoneResp, 0, sizeof(sc_PersonalDBEditDoneResp));

	/** @brief 파일 해쉬값 저장용 */
	unsigned char fileHash[32];

	/** @brief DB 파일 위한 구조체 */
	FILE *infoFile;

	//DB파일 오픈
	if (fopen_s(&infoFile, "./myinfoClient.db", "rb")) {
		printDebugMsg(DC_ERROR, errorLevel, "파일을 읽기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//해쉬 구하기
	getFileHash(infoFile, fileHash);

	if (!memcmp(fileHash, originalHash, 32)) {//만약에 파일이 수정되지 않았다면
		fclose(infoFile);//파일 닫기
		return;//그냥 종료한다
	}

	//수정된 파일이라면 계속 진행한다.
	
	//패킷 설정
	PersonalDBEditDone.Data.opCode = htonl(OP_CS_PERSONALDBEDITDONE);
	PersonalDBEditDone.Data.dataLen = htonl(sizeof(cs_PersonalDBEditDone) - 8);
	memcpy(PersonalDBEditDone.Data.hash, fileHash, 32);

	/** @brief 업로드 성공 유무 플래그 */
	int flag = 0;

	/** @brief 업로드 횟수 카운트 */
	int count = 0;

	while (1) {//업로드 횟수 채울때 까지 반복시킴
		puts("\n유저 정보 업로드 시작...");
		count++;//카운트 증가

		//패킷 전송
		sendData(hSocket, PersonalDBEditDone.buf, sizeof(cs_PersonalDBEditDone), 0);

		//업로드 모드 진입
		fseek(infoFile, 0, SEEK_END);

		/** @brief 업로드할 파일 사이즈 */
		unsigned int fileSize = ftell(infoFile);

		/** @brief 업로드할 남은 용량 */
		unsigned int left = fileSize;

		/** @brief 읽어올 바이트 수 */
		unsigned int toRead;

		fseek(infoFile, 0, SEEK_SET);

		//파일 사이즈 전송
		fileSize = htonl(fileSize);
		sendData(hSocket, (char*)&fileSize, 4, 0);
		fileSize = ntohl(fileSize);

		/**
			@var unsigned char dataBuffer[4096]
			업로드 버퍼 4KiB
		*/
		unsigned char dataBuffer[4096];

		while (1) {//업로드 끝날때 까지 반복
			if (left < 4096U)//4KB보다 작은만큼 남으면
				toRead = left;//남은 만큼 보냄
			else//아니면
				toRead = 4096U;//4KiB만큼 보냄

			//파일 읽어서 버퍼에 저장
			fread(dataBuffer, toRead, 1, infoFile);

			//읽어온 버퍼를 서버로 전송
			if (!sendRaw(hSocket, dataBuffer, toRead, 0)) {
				printDebugMsg(DC_ERROR, errorLevel, "전송 실패");
				printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
				system("pause");
				exit(1);
			}

			left -= toRead;//보낸만큼 뺀다
			updateProgress(fileSize - left, fileSize);//프로그레스 바 업데이트(생성)
			if (!left) break;//완료시 탈출
		}

		//업로드 모드 끝

		//업로드 완료 패킷 가져옴
		recvData(hSocket, PersonalDBEditDoneResp.buf, sizeof(sc_PersonalDBEditDoneResp), 0);

		//숫자를 호스트 특정 인디안으로 변경
		PersonalDBEditDoneResp.Data.opCode = ntohl(PersonalDBEditDoneResp.Data.opCode);
		PersonalDBEditDoneResp.Data.dataLen = ntohl(PersonalDBEditDoneResp.Data.dataLen);

		if (PersonalDBEditDoneResp.Data.statusCode)//성공시 탈출
			break;

		if (!flag && count == 3) {//3번 다 실패한 경우
			printDebugMsg(DC_ERROR, errorLevel, "중계서버에 파일을 저장할 수 없었습니다.");
			printDebugMsg(DC_ERROR, errorLevel, "myinfoClient.db 파일을 수동으로 백업해 주십시오.");
			printDebugMsg(DC_ERROR, errorLevel, "그 후 관리자에게 이메일로 전송해 주시면 수동으로 적용됩니다.");
			printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
			system("pause");
			exit(1);
		}
	}
	fclose(infoFile);
	puts("\n유저 정보 업로드 완료");
	system("pause");
	return;
}

void addContacts() {
	DC_CONTACTS tmpContacts;
	memset(&tmpContacts, 0, sizeof(DC_CONTACTS));

	//연락처 사용자로부터 받아옴
	printf_s("이름(최대 한글 15자, 알파벳 및 숫자 30자, 취소는 아무것도 입력하지 않고 엔터): ");
	scanf_s("%30[^\n]", tmpContacts.name, 31);
	clearStdinBuffer();
	if (strlen(tmpContacts.name) == 0) return; //이름이 없으면 종료

	printf_s("이메일(최대 50자) ");
	scanf_s("%50[^\n]", tmpContacts.email, 51);
	clearStdinBuffer();
	printf_s("연락처1(최대 20자) ");
	scanf_s("%20[^\n]", tmpContacts.phone1, 21);
	clearStdinBuffer();
	printf_s("연락처2(최대 20자) ");
	scanf_s("%20[^\n]", tmpContacts.phone2, 21);
	clearStdinBuffer();
	printf_s("연락처3(최대 20자) ");
	scanf_s("%20[^\n]", tmpContacts.phone3, 21);
	clearStdinBuffer();

	/** @brief sqlite3 핸들 구조체 포인터 */
	sqlite3 *dbHandle;

	/** @brief 연락처 저장을 위한 쿼리 */
	char *insertMemo = "INSERT into contacts VALUES(NULL, '%q', '%q', '%q', '%q', '%q');";

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, errorLevel, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	/** @brief 실행하기 위한 완성된 쿼리 */
	char* query = sqlite3_mprintf(insertMemo, tmpContacts.email, tmpContacts.name, tmpContacts.phone1, tmpContacts.phone2, tmpContacts.phone3);

	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//쿼리문 실행 => db 삽입
	sqlite3_close(dbHandle);//db파일 닫기

	//동적 할당 변수 free
	sqlite3_free(query);

	return;
};

/**
@fn void modifyContacts(int count)
@brief DogeCloud DB에 연락처 수정
@author 멍멍아야옹해봐
@param count 현재 연락처 개수
*/
void modifyContacts(int count) {
	DC_CONTACTS tmpContacts;

	/** @brief 업데이트 쿼리 */
	char *selectContacts = "SELECT * FROM contacts WHERE id = %d;";
	/** @brief 업데이트 쿼리 */
	char *updateContacts = "UPDATE contacts SET name = '%q', email = '%q', phone1 = '%q', phone2 = '%q', phone3 = '%q' WHERE id = %d;";

	//유저에게 입력 받음
	int contactsID;
	printf_s("몇 번 연락처를 수정하시겠습니까? (취소: 0): ");
	scanf_s("%d", &contactsID);
	clearStdinBuffer();

	if (contactsID == 0) return;
	if (contactsID < 0 || contactsID > count) {
		puts("유효하지 않은 입력입니다.");
		system("pause");
		return;
	}

	/** @brief sqlite3을 위한 handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, errorLevel, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	sqlite3_stmt* stmt;
	char *selectQueryRdy = sqlite3_mprintf(selectContacts, contactsID);
	if (sqlite3_prepare(dbHandle, selectQueryRdy, -1, &stmt, NULL) == SQLITE_OK)
	{
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			memcpy(tmpContacts.name, sqlite3_column_text(stmt, 1), 31);
			memcpy(tmpContacts.email, sqlite3_column_text(stmt, 1), 51);
			memcpy(tmpContacts.phone1, sqlite3_column_text(stmt, 1), 21);
			memcpy(tmpContacts.phone2, sqlite3_column_text(stmt, 1), 21);
			memcpy(tmpContacts.phone3, sqlite3_column_text(stmt, 1), 21);
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_free(selectQueryRdy);

	int userInput;
	do {
		system("cls");
		printf_s("\n******************************************************************************************");
		printf_s("\n %16s %30s %20s %20s %20s", "이름", "이메일", "연락처1", "연락처2", "연락처3");
		printf_s("\n %16s %30s %20s %20s %20s", tmpContacts.name, tmpContacts.email, tmpContacts.phone1, tmpContacts.phone2, tmpContacts.phone3);
		printf_s("\n******************************************************************************************");
		puts("\n수정할 것을 선택 해 주세요.");
		puts("0: 종료 1:이름 2:이메일 3:연락처1 4:연락처2 5:연락처3");
		puts("\n******************************************************************************************");
		printf_s("=> ");
		scanf_s("%d", &userInput);
		clearStdinBuffer();

		char tmpName[31] = { 0, };
		
		switch (userInput) {
		case 0:
			break;
		case 1:
			printf_s("이름(최대 한글 15자, 알파벳 및 숫자 30자, 취소는 아무것도 입력하지 않고 엔터): ");
			scanf_s("%30[^\n]", tmpName, 31);
			clearStdinBuffer();
			if (!strlen(tmpName)) break;
			memcpy(tmpContacts.name, tmpName, 31);
			break;
		case 2:
			printf_s("이메일(최대 50자) ");
			scanf_s("%50[^\n]", tmpContacts.email, 51);
			clearStdinBuffer();
			break;
		case 3:
			printf_s("연락처1(최대 20자) ");
			scanf_s("%20[^\n]", tmpContacts.phone1, 21);
			clearStdinBuffer();
			break;
		case 4:
			printf_s("연락처2(최대 20자) ");
			scanf_s("%20[^\n]", tmpContacts.phone2, 21);
			clearStdinBuffer();
			break;
		case 5:
			printf_s("연락처3(최대 20자) ");
			scanf_s("%20[^\n]", tmpContacts.phone3, 21);
			clearStdinBuffer();
			break;
		default:
			printDebugMsg(DC_WARN, errorLevel, "올바르지 않은 입력입니다.");
			Sleep(1000);
			break;
		}

	} while (userInput != 0);

	/** @brief 완성된 삭제 쿼리 */
	char* query = sqlite3_mprintf(updateContacts, tmpContacts.name, tmpContacts.email, tmpContacts.phone1, tmpContacts.phone2, tmpContacts.phone3, contactsID);
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//실행
	sqlite3_free(query);//동적할당 free
	sqlite3_close(dbHandle);//db 종료

	return;
}

/**
@fn void deleteContacts(int count)
@brief DogeCloud DB에 연락처 삭제
@author 멍멍아야옹해봐
@param count 현재 연락처 개수
*/
void deleteContacts(int count) {

	/** @brief 삭제용 쿼리 */
	char *deleteMemo = "DELETE FROM contacts WHERE id = %d;";
	/** @brief 현재 줄 인덱스 값 확인 쿼리 */
	char *selectIndex = "SELECT id FROM contacts LIMIT 1 OFFSET %d;";
	/** @brief 인덱스 재설정 쿼리 */
	char *updateIndex = "UPDATE contacts SET id = %d WHERE id = %d;";
	/** @brief auto increment 재설정 쿼리 */
	char *reIndex = "delete from sqlite_sequence where name='contacts';";

	//유저에게 입력 받음
	int contactsID;
	printf_s("몇 번 연락처를 삭제하시겠습니까? (취소: 0): ");
	scanf_s("%d", &contactsID);
	clearStdinBuffer();

	if (contactsID == 0) return;
	if (contactsID > count || contactsID < 0) {
		puts("유효하지 않은 입력입니다.");
		system("pause");
		return;
	}

	unsigned char userInput;
	do {
		printf_s("정말 %d번 연락처를 삭제하시겠습니까? (Y/N): ", contactsID);
		scanf_s("%c", &userInput, 1);
		clearStdinBuffer();
	} while (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n');
	if (userInput == 'N' || userInput == 'n') return;
	//유저 입력 끝

	/** @brief sqlite3을 위한 handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, errorLevel, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	/** @brief 완성된 삭제 쿼리 */
	char* query = sqlite3_mprintf(deleteMemo, contactsID);
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//실행
	sqlite3_free(query);//동적할당 free

	//인덱스값 다시 설정

	/** @brief sqlite3을 위한 stmt */
	sqlite3_stmt* stmt;
	for (int i = contactsID; i < count; i++) {//삭제한 부분부터 시작
		char *selIdxReady = sqlite3_mprintf(selectIndex, i - 1);
		if (sqlite3_prepare(dbHandle, selIdxReady, -1, &stmt, NULL) == SQLITE_OK)
		{
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				/** @brief 인덱스 업데이트를 위한 완성된 쿼리 */
				char *updateIdxReady = sqlite3_mprintf(updateIndex, i, sqlite3_column_int(stmt, 0));//줄 번호 = 인덱스 값
				sqlite3_exec(dbHandle, updateIdxReady, NULL, NULL, NULL);
				sqlite3_free(updateIdxReady);
			}
		}
		sqlite3_finalize(stmt);
		sqlite3_free(selIdxReady);//동적할당 해제
	}
	sqlite3_exec(dbHandle, reIndex, NULL, NULL, NULL);//auto increment 값 다시 설정

	sqlite3_close(dbHandle);//db 종료

	return;
}

/**
	@fn void addMemo()
	@brief DogeCloud DB에 메모 추가
	@author 멍멍아야옹해봐
*/
void addMemo() {
	/** @brief 임시로 생성할 파일 이름 */
	char tmpFileName[65] = { 0, };

	/** @brief 임시로 생성할 숫자 */
	unsigned char tmpRandomNum[16] = { 0, };

	/** @brief 임시로 생성된 숫자의 해쉬 저장용 */
	unsigned char tmpHash[32] = { 0, };

	/** @brief 실행할 커맨드 저장용 */
	char cmd[255] = { 0, };

	/** @brief 임시로 생성할 파일을 위한 구조체 포인터 */
	FILE *tmpFile;

	/** @brief sqlite3 핸들 구조체 포인터 */
	sqlite3 *dbHandle;

	/** @brief 메모 저장을 위한 쿼리 */
	char *insertMemo = "INSERT into memo VALUES(NULL, '%q');";

	//임시로 사용할 파일이름 생성
	GenerateCSPRNG(tmpRandomNum, 15);
	SHA256_Text(tmpRandomNum, tmpHash);
	for (int i = 0; i < 32; i++)
		sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

	//임시파일 생성
	if (fopen_s(&tmpFile, tmpFileName, "wb+")) {
		printDebugMsg(DC_ERROR, errorLevel, "파일을 쓰기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}
	fclose(tmpFile);//수정을 위해 외부 프로그램이 열 수 있도록 파일 해제

	//메모장 실행
	sprintf_s(cmd, 255, "notepad %s", tmpFileName);
	puts("수정이 완료되면 저장하고 메모장을 닫아주세요.");
	system(cmd);

	if (fopen_s(&tmpFile, tmpFileName, "rb")) {//DB저장을 위해서 다시 파일을 연다
		printDebugMsg(DC_ERROR, errorLevel, "파일을 읽기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, errorLevel, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	/** @brief 메모 파일 사이즈 */
	unsigned long memoFileSize;
	fseek(tmpFile, 0, SEEK_END);
	memoFileSize = ftell(tmpFile);

	if (memoFileSize == 0) {//만약 파일 사이즈가 0이면 아무 작업도 없는것
		sqlite3_close(dbHandle);
		fclose(tmpFile);
		remove(tmpFileName);//임시파일 삭제
		return;
	}

	rewind(tmpFile);//처음 부분으로 이동

	//쿼리문에 넣기 위해 메모 크기만큼 동적할당
	char* text = (void*)calloc(1, memoFileSize+1);

	if (text == NULL) {
		printDebugMsg(DC_ERROR, errorLevel, "메모리 할당에 실패했습니다.");
		return;
	}

	/** @brief 앞으로 읽어와야 하는 크기 */
	unsigned long left = memoFileSize;
	/** @brief 얼마만큼 읽어올지 */
	unsigned long toRead;

	while (1) {//다 읽을 때 까지 반복
		if (left < 4096U)//4KB보다 작은만큼 남으면
			toRead = left;//남은 만큼 읽음
		else//아니면
			toRead = 4096U;//4KiB만큼 읽음
		//파일 읽어서 버퍼에 저장
		fread(text + (memoFileSize - left), toRead, 1, tmpFile);

		left -= toRead;//보낸만큼 뺀다
		updateProgress(memoFileSize - left, memoFileSize);//프로그레스 바 업데이트(생성)
		if (!left) break;//완료시 탈출
	}

	/** @brief 실행하기 위한 완성된 쿼리 */
	char* query = sqlite3_mprintf(insertMemo, text);
	
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//쿼리문 실행 => db 삽입

	fclose(tmpFile);//파일 해제
	remove(tmpFileName);//임시파일 삭제
	sqlite3_close(dbHandle);//db파일 닫기

	//동적 할당 변수 free
	free(text);
	sqlite3_free(query);

	return;
}

/**
@fn void modifyMemo(int count)
@brief DogeCloud DB에 메모 수정
@author 멍멍아야옹해봐
@param count 현재 메모 개수
*/
void modifyMemo(int count) {
	/** @brief 임시로 생성할 파일 이름 */
	char tmpFileName[65] = { 0, };

	/** @brief 임시로 생성할 숫자 */
	unsigned char tmpRandomNum[16] = { 0, };

	/** @brief 임시로 생성된 숫자의 해쉬 저장용 */
	unsigned char tmpHash[32] = { 0, };

	/** @brief 실행할 커맨드 저장용 */
	char cmd[255] = { 0, };

	/** @brief 임시로 생성할 파일을 위한 구조체 포인터 */
	FILE *tmpFile;

	/** @brief sqlite3 핸들 구조체 포인터 */
	sqlite3 *dbHandle;

	/** @brief 수정할 메모를 찾아오는 쿼리 */
	char *selectMemo = "SELECT user_text FROM memo WHERE id = %d;";
	/** @brief 메모 수정을 위한 쿼리 */
	char *modifyMemo = "UPDATE memo SET user_text = '%q' WHERE id = %d;";

	//유저에게 입력 받음
	int memoID;
	printf_s("몇 번 메모를 수정하시겠습니까? (취소: 0): ");
	scanf_s("%d", &memoID);
	clearStdinBuffer();

	if (memoID == 0) return;
	else if (memoID > count || memoID < 0) {
		puts("유효하지 않은 입력입니다.");
		system("pause");
		return;
	}
	//유저 입력 끝

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, errorLevel, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//임시로 사용할 파일이름 생성
	GenerateCSPRNG(tmpRandomNum, 15);
	SHA256_Text(tmpRandomNum, tmpHash);
	for (int i = 0; i < 32; i++)
		sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

	//임시파일 생성
	if (fopen_s(&tmpFile, tmpFileName, "wb+")) {
		printDebugMsg(DC_ERROR, errorLevel, "파일을 쓰기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	sqlite3_stmt* stmt;
	char *selectQueryRdy = sqlite3_mprintf(selectMemo, memoID);
	if (sqlite3_prepare(dbHandle, selectQueryRdy, -1, &stmt, NULL) == SQLITE_OK)
	{
		if (sqlite3_step(stmt) == SQLITE_ROW) {
			const unsigned char *user_text = sqlite3_column_text(stmt, 0);
			unsigned long orgMemoFileSize = (unsigned long)strlen(user_text) + 1;
			unsigned long left = orgMemoFileSize;

			/** @brief 얼마만큼 쓸지 */
			unsigned long toWrite;

			while (1) {//다 쓸 때 까지 반복
				if (left < 4096U)//4KB보다 작은만큼 남으면
					toWrite = left;//남은 만큼 작성
				else//아니면
					toWrite = 4096U;//4KiB만큼 읽음
				//파일 읽고 버퍼에 저장
				fwrite(user_text + (orgMemoFileSize - left), toWrite, 1, tmpFile);

				left -= toWrite;//쓴만큼 뺀다
				updateProgress(orgMemoFileSize - left, orgMemoFileSize);//프로그레스 바 업데이트(생성)
				if (!left) break;//완료시 탈출
			}
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_free(selectQueryRdy);

	unsigned char orgHash[32] = { 0, };
	getFileHash(tmpFile, orgHash);

	fclose(tmpFile);//수정을 위해 외부 프로그램이 열 수 있도록 파일 해제
	
	//메모장 실행
	sprintf_s(cmd, 255, "notepad %s", tmpFileName);
	puts("\n수정이 완료되면 저장하고 메모장을 닫아주세요.");
	system(cmd);

	if (fopen_s(&tmpFile, tmpFileName, "rb")) {//DB읽기를 위해서 파일을 연다
		printDebugMsg(DC_ERROR, errorLevel, "파일을 읽기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	/** @brief 수정된 메모 파일 사이즈 */
	unsigned long newMemoFileSize;
	fseek(tmpFile, 0, SEEK_END);
	newMemoFileSize = ftell(tmpFile);

	unsigned char newHash[32];
	getFileHash(tmpFile, newHash);

	if (!memcmp(newHash, orgHash, 32)) {//만약 파일 해시가 같다면 그냥 끝낸다
		sqlite3_close(dbHandle);
		fclose(tmpFile);
		remove(tmpFileName);//임시파일 삭제
		return;
	}

	rewind(tmpFile);//처음 부분으로 이동

	//쿼리문에 넣기 위해 메모 크기만큼 동적할당
	char* newText = (void*)calloc(1, newMemoFileSize + 1);

	if (newText == NULL) {
		printDebugMsg(DC_ERROR, errorLevel, "메모리 할당에 실패했습니다.");
		return;
	}

	/** @brief 앞으로 읽어와야 하는 크기 */
	unsigned long left = newMemoFileSize;
	/** @brief 얼마만큼 읽어올지 */
	unsigned long toRead;

	while (1) {//다 읽을 때 까지 반복
		if (left < 4096U)//4KB보다 작은만큼 남으면
			toRead = left;//남은 만큼 읽음
		else//아니면
			toRead = 4096U;//4KiB만큼 읽음
		//파일 읽어서 버퍼에 저장
		fread(newText + (newMemoFileSize - left), toRead, 1, tmpFile);

		left -= toRead;//보낸만큼 뺀다
		updateProgress(newMemoFileSize - left, newMemoFileSize);//프로그레스 바 업데이트(생성)
		if (!left) break;//완료시 탈출
	}

	/** @brief 실행하기 위한 완성된 쿼리 */
	char* updateQueryRdy = sqlite3_mprintf(modifyMemo, newText, memoID);

	sqlite3_exec(dbHandle, updateQueryRdy, NULL, NULL, NULL);//쿼리문 실행 => db 업데이트

	fclose(tmpFile);//파일 해제
	remove(tmpFileName);//임시파일 삭제
	sqlite3_close(dbHandle);//db파일 닫기

	//동적 할당 변수 free
	free(newText);
	sqlite3_free(updateQueryRdy);

	return;
}

/**
@fn void deleteMemo(int count)
@brief DogeCloud DB에 메모 삭제
@author 멍멍아야옹해봐
@param count 현재 메모 개수
*/
void deleteMemo(int count) {

	/** @brief 삭제용 쿼리 */
	char *deleteMemo = "DELETE FROM memo WHERE id = %d;";
	/** @brief 현재 줄 인덱스 값 확인 쿼리 */
	char *selectIndex = "SELECT id FROM memo LIMIT 1 OFFSET %d;";
	/** @brief 인덱스 재설정 쿼리 */
	char *updateIndex = "UPDATE memo SET id = %d WHERE id = %d;";
	/** @brief auto increment 재설정 쿼리 */
	char *reIndex = "delete from sqlite_sequence where name='memo';";

	//유저에게 입력 받음
	int memoID;
	printf_s("몇 번 메모를 삭제하시겠습니까? (취소: 0): ");
	scanf_s("%d", &memoID);
	clearStdinBuffer();

	if (memoID == 0) return;
	else if (memoID > count || memoID < 0) {
		puts("유효하지 않은 입력입니다.");
		system("pause");
		return;
	}

	unsigned char userInput;
	do {
		printf_s("정말 %d번 메모를 삭제하시겠습니까? (Y/N): ", memoID);
		scanf_s("%c", &userInput, 1);
		clearStdinBuffer();
	} while (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n');
	if (userInput == 'N' || userInput == 'n') return;
	//유저 입력 끝

	/** @brief sqlite3을 위한 handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, errorLevel, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	/** @brief 완성된 삭제 쿼리 */
	char* query = sqlite3_mprintf(deleteMemo, memoID);
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//실행
	sqlite3_free(query);//동적할당 free

	//인덱스값 다시 설정

	/** @brief sqlite3을 위한 stmt */
	sqlite3_stmt* stmt;
	for (int i = memoID; i < count; i++) {//삭제한 부분부터 시작
		char *selIdxReady = sqlite3_mprintf(selectIndex, i - 1);
		if (sqlite3_prepare(dbHandle, selIdxReady, -1, &stmt, NULL) == SQLITE_OK)
		{
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				/** @brief 인덱스 업데이트를 위한 완성된 쿼리 */
				char *updateIdxReady = sqlite3_mprintf(updateIndex, i, sqlite3_column_int(stmt, 0));//줄 번호 = 인덱스 값
				sqlite3_exec(dbHandle, updateIdxReady, NULL, NULL, NULL);
				sqlite3_free(updateIdxReady);
			}
		}
		sqlite3_finalize(stmt);
		sqlite3_free(selIdxReady);//동적할당 해제
	}
	sqlite3_exec(dbHandle, reIndex, NULL, NULL, NULL);//auto increment 값 다시 설정

	sqlite3_close(dbHandle);//db 종료

	return;
}