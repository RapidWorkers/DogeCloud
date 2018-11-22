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
	@file userDataHelper.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief 파일, 연락처, 메모 관리 메뉴 출력용 함수 모음
*/

#include "DogeCloud.h"
#include <sqlite3.h>

/**
	@fn void manageContacts(SOCKET hSocket)
	@brief DogeCloud 연락처 처리 함수
	@author 멍멍아야옹해봐
	@param hSocket 중계서버 연결된 소켓
*/
void manageContacts(SOCKET hSocket) {
	/** @brief DB 파일 해쉬 저장용 */
	char originalHash[32];

	/** @brief db 읽어오기 위한 파일 구조체 포인터 */
	FILE *fp;

	system("cls");

	//DB파일 열기
	if (fopen_s(&fp, "./myinfoClient.db", "rb")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	//해쉬 구하기
	getFileHash(fp, originalHash);//나중에 변경 확인을 위해 먼저 파일 해쉬를 저장함

	//sqlite에서 열기 위해 파일 닫음
	fclose(fp); 

	/** @brief 최대 페이지 수 */
	int maxpage = 2;

	/** @brief 현재 페이지 */
	int page = 1;

	while (1) {//종료될 때 까지 반복
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

		//메뉴 선택
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://추가
			addContacts();
			break;
		case 2://수정
			modifyContacts();
			break;
		case 3://이전 페이지
			if (maxpage > page) page++;
			break;
		case 4://다음 페이지
			if (page > 1) page--;
			break;
		case 5://나가기
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

/**
	@fn void manageMemo(SOCKET hSocket)
	@brief DogeCloud 메모 처리 함수
	@author 멍멍아야옹해봐
	@param hSocket 중계서버 연결된 소켓
*/
void manageMemo(SOCKET hSocket) {
	/** @brief DB 파일 해쉬 저장용 */
	unsigned char originalHash[32];

	/** @brief 파일 읽어오기 위한 구조체 포인터 */
	FILE *fp;

	system("cls");

	//DB파일 열기
	if (fopen_s(&fp, "./myinfoClient.db", "rb")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	getFileHash(fp, originalHash);//나중에 변경 확인을 위해 먼저 파일 해쉬를 저장함

	//sqlite에서 열기 위해 파일 닫음
	fclose(fp);

	/** @brief sqlite3을 위한 handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	char* countQuery = "SELECT count(id) FROM memo;";
	char* selectMemoQuery = "SELECT * FROM memo LIMIT 10 OFFSET %d;";

	/** @brief 메모 개수 */
	int count = 0;
	/** @brief 현재 페이지 */
	int page = 1;

	while (1) {//종료될 때 까지 반복

		sqlite3_stmt* stmt;
		if (sqlite3_prepare(dbHandle, countQuery, -1, &stmt, NULL) == SQLITE_OK)
		{
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				count = sqlite3_column_int(stmt, 0);
			}
			else {
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
				system("pause");
				exit(1);
			}
		}
		sqlite3_finalize(stmt);

		/** @brief 최대 페이지 수 */
		int maxpage = (count-1) / 10 + 1;

		//메모 출력
		system("cls");
		printf_s("\n******************************************************************************************");
		if (count == 0) {
			printf_s("\n%80s", "저장된 메모가 없습니다.");
		}
		else {
			printf_s("\n%4s %60s", "순번", "내용 일부 (한글 기준 최대 30자)");
			char *query = sqlite3_mprintf(selectMemoQuery, (page - 1) * 10);
			if (sqlite3_prepare(dbHandle, query, -1, &stmt, NULL) == SQLITE_OK)
			{
				while (sqlite3_step(stmt) == SQLITE_ROW) {
					printf_s("\n%4d %60.60s", sqlite3_column_int(stmt, 0), (char*)sqlite3_column_text(stmt, 1));
				}
			}
			sqlite3_finalize(stmt);
			sqlite3_free(query);
		}
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

		//메뉴 선택
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://추가
			addMemo();
			break;
		case 2://보기 및 수정
			modifyMemo(count);
			break;
		case 3://삭제
			deleteMemo(count);
			break;
		case 4://이전 페이지
			if (page > 1) page--;
			break;
		case 5://다음 페이지
			if (maxpage > page) page++;
			break;
		case 6://나가기
			sqlite3_close(dbHandle);
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


/**
	@fn void manageFile(SOCKET hSocket)
	@brief DogeCloud 파일서버 처리 함수
	@author 멍멍아야옹해봐
	@param hSocket 중계서버 연결된 소켓
*/
void manageFile(SOCKET hSocket) {

	//패킷 선언 및 초기화
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	//패킷 설정
	FileSrvConReq.Data.opCode = htonl(OP_CS_FILESRVCONNREQ);
	FileSrvConReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	//패킷 전송 및 수신
	sendData(hSocket, FileSrvConReq.buf, sizeof(cs_FileSrvConReq), 0);
	recvData(hSocket, FileSrvConResp.buf, sizeof(sc_FileSrvConResp), 0);

	//호스트 인디안으로 변경
	FileSrvConResp.Data.opCode = ntohl(FileSrvConResp.Data.opCode);
	FileSrvConResp.Data.dataLen = ntohl(FileSrvConResp.Data.dataLen);
	
	//실패 검사
	if (!FileSrvConResp.Data.statusCode) {
		puts("파일서버 정보를 가져올 수 없습니다!");
		puts("파일 서버에 문제가 있거나 잠시 처리가 지연되는 중일 수 있습니다. 나중에 다시 시도해주세요.");
		system("pause");
		return;
	}

	//파일서버 열기
	openFileServer(FileSrvConResp.Data.fileSrvAddr, FileSrvConResp.Data.fileSrvPort, FileSrvConResp.Data.authKey);
	return;
}