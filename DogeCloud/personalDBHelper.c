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
#include <stdlib.h>

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

	/**
		@var FILE *downFile
		다운로드 받을 파일을 위한 구조체
	*/
	FILE *downFile;

	/**
		@var unsigned int toWrite
		받아올 버퍼 크기
	*/
	unsigned int toWrite;

	/**
		@var unsigned long fileSize
		받아올 파일 크기
	*/
	unsigned long fileSize;

	/**
		@var unsigned char dataBuffer[4096]
		다운로드 버퍼 4KiB
	*/
	unsigned char dataBuffer[4096];

	/**
		@var unsigned char fileHash[32]
		파일 해쉬값 저장용
	*/
	unsigned char fileHash[32];

	//패킷 설정
	DownloadInfoReq.Data.opCode = htonl(OP_CS_DOWNLOADUSERINFOREQ);
	DownloadInfoReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	//다운로드 요청 패킷 전송
	sendData(hSocket, DownloadInfoReq.buf, sizeof(cs_DownloadPersonalDBReq), 0);

	//다운로드 모드 시작
	if (fopen_s(&downFile, "myinfoClient.db", "wb+")) {//DB 파일 다운로드를 위해 파일 열기
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 쓰기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	recvData(hSocket, &fileSize, 4, 0);
	fileSize = ntohl(fileSize);//호스트 특정 인디안으로 변환

	/**
		@var unsigned long left
		남은 파일 크기
	*/
	unsigned long left = fileSize;

	puts("\n유저 정보 다운로드 시작...");

	while (1) {//남은 크기가 0이 될때 까지 반복
		if (left < 4096U)//4KB보다 작은만큼 남으면
			toWrite = left;//남은 만큼 받아옴
		else//아니면
			toWrite = 4096U;//4KiB만큼 받아옴

		if (!recvRaw(hSocket, dataBuffer, toWrite, 0)) {//서버로부터 다운로드
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "전송 실패");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
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

	if (!memcmp(fileHash, DownloadInfoResp.Data.hash, 32)) {//만약 해쉬값이 다르다면
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터 불일치");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
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

	/**
		@var unsigned char fileHash[32]
		파일 해쉬값 저장용
	*/
	unsigned char fileHash[32];

	/**
		@var FILE *infoFile
		DB 파일 위한 구조체
	*/
	FILE *infoFile;

	//DB파일 오픈
	if (fopen_s(&infoFile, "./myinfoClient.db", "rb")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 읽기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
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

	/**
		@var int flag
		업로드 성공 유무 플래그
	*/
	int flag = 0;

	/**
		@var int count
		업로드 횟수 카운트
	*/
	int count = 0;

	while (1) {//업로드 횟수 채울때 까지 반복시킴
		puts("\n유저 정보 업로드 시작...");
		count++;//카운트 증가

		//패킷 전송
		sendData(hSocket, PersonalDBEditDone.buf, sizeof(cs_PersonalDBEditDone), 0);

		//업로드 모드 진입
		fseek(infoFile, 0, SEEK_END);

		/**
			@var unsigned int fileSize
			업로드할 파일 사이즈
		*/
		unsigned int fileSize = ftell(infoFile);

		/**
			@var unsigned int left
			업로드할 남은 용량
		*/
		unsigned int left = fileSize;

		/**
			@var unsigned int toRead
			읽어올 바이트 수
		*/
		unsigned int toRead;

		fseek(infoFile, 0, SEEK_SET);

		//파일 사이즈 전송
		fileSize = htonl(fileSize);
		sendData(hSocket, &fileSize, 4, 0);
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
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "전송 실패");
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
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
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버에 파일을 저장할 수 없었습니다.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "myinfoClient.db 파일을 수동으로 백업해 주십시오.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "그 후 관리자에게 이메일로 전송해 주시면 수동으로 적용됩니다.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
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

};

void modifyContacts() {

}

void deleteContacts() {

}

/**
	@fn void addMemo()
	@brief DogeCloud DB에 메모 추가
	@author 멍멍아야옹해봐
*/
void addMemo() {
	/**
		@var char tmpFileName[65]
		임시로 생성할 파일 이름
	*/
	char tmpFileName[65] = { 0, };

	/**
		@var unsigned char tmpRandomNum[16]
		임시로 생성할 숫자
	*/
	unsigned char tmpRandomNum[16] = { 0, };

	/**
		@var unsigned char tmpHash[32]
		임시로 생성된 숫자의 해쉬 저장용
	*/
	unsigned char tmpHash[32] = { 0, };

	/**
		@var char cmd[255]
		실행할 커맨드 저장용
	*/
	char cmd[255] = { 0, };

	/**
		@var FILE *tmpFile
		임시로 생성할 파일을 위한 구조체 포인터
	*/
	FILE *tmpFile;

	/**
		@var sqlite3 *dbHandle
		sqlite3 핸들 구조체 포인터
	*/
	sqlite3 *dbHandle;

	//메모 저장을 위한 쿼리
	char *insertMemo = "INSERT into memo VALUES(NULL, '%q');";

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

	//메모장 실행
	sprintf_s(cmd, 255, "notepad %s", tmpFileName);
	puts("수정이 완료되면 저장하고 메모장을 닫아주세요.");
	system(cmd);

	if (fopen_s(&tmpFile, tmpFileName, "rb")) {//DB저장을 위해서 다시 파일을 연다
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 읽기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB 오픈
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터베이스 파일을 읽을 수 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	unsigned long memoFileSize;
	fseek(tmpFile, 0, SEEK_END);
	memoFileSize = ftell(tmpFile);

	if (memoFileSize == 0) {//만약 파일 사이즈가 0이면 아무 작업도 없는것
		sqlite3_close(dbHandle);
		fclose(tmpFile);
		remove(tmpFileName);//임시파일 삭제
		return;
	}

	rewind(tmpFile);
	char* text = (void*)calloc(1, memoFileSize+1);
	unsigned long left = memoFileSize;
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

	char* query = sqlite3_mprintf(insertMemo, text);
	
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//db에 삽입

	fclose(tmpFile);//파일 해제
	remove(tmpFileName);//임시파일 삭제
	sqlite3_close(dbHandle);//db파일 닫기

	//동적 할당 변수 free
	free(text);
	sqlite3_free(query);

	return;
}

void modifyMemo() {

}

void deleteMemo() {

}