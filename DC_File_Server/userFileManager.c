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
	@file userFileManager.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief 사용자 파일서버 작업 관련 함수
*/

#include "FileServer.h"

/**
	@fn void procListFile(SOCKET hClientSock)
	@brief 파일 리스트 전송
	@author 멍멍아야옹해봐
	@param hClientSock 클라이언트 연결된 소켓
*/
void procListFile(SOCKET hClientSock) {
	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	recvData(hClientSock, ListFile.buf + 4, sizeof(cf_ListFile) - 4, 0);

	int page = ListFile.Data.page;

	ListFileResp.Data.opCode = htonl(OP_FC_LISTFILERESP);
	ListFileResp.Data.dataLen = htonl(sizeof(fc_ListFileResp) - 8);
	ListFileResp.Data.statusCode = 1;

	int userUID = 0;
	char* currentDir[255] = { 0, };
	WaitForSingleObject(hMutex, INFINITE);
			for (int j = 0; j < clientCount; j++) {
				if (hClientSock == hClientSocks[j]) {
					userUID = sessionList[j].userUID;
					break;
				}
		}
	ReleaseMutex(hMutex);

	//파일 개수 확인
	char *getFileCountQuery = "SELECT count(id) FROM fileList WHERE owner = ?";

	//쿼리 바인더 준비
	MYSQL_BIND bind_getFileCount_set[1];
	memset(bind_getFileCount_set, 0, sizeof(bind_getFileCount_set));
	bind_getFileCount_set[0].buffer_type = MYSQL_TYPE_LONG;
	bind_getFileCount_set[0].buffer = &userUID;
	bind_getFileCount_set[0].buffer_length = 4;

	//결과 바인더 준비
	int fileCount = 0;
	MYSQL_BIND bind_getFileCount_res[1];
	memset(&bind_getFileCount_res, 0, sizeof(bind_getFileCount_res)); //init bind
	bind_getFileCount_res[0].buffer_type = MYSQL_TYPE_LONG;
	bind_getFileCount_res[0].buffer = &fileCount;
	bind_getFileCount_res[0].buffer_length = 4;

	//MySQL Prepared Statement 초기화
	MYSQL_STMT *stmt;
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, errorLevel, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL 실행
	sqlPrepareAndExecute(&sqlHandle, stmt, getFileCountQuery, bind_getFileCount_set, bind_getFileCount_res);

	//결과물 가져오기
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, errorLevel, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	mysql_stmt_free_result(stmt);

	//파일 개수 패킷에 복사
	ListFileResp.Data.fileCount = fileCount;

	int totalPage = (fileCount - 1) / 10 + 1;
	ListFileResp.Data.totalPage = totalPage;

	if (page > totalPage) page = totalPage;
	else if (page < 1) page = 1;

	ListFileResp.Data.currentPage = page;

	if(fileCount == 0){//파일 개수가 0개면

		sendData(hClientSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);
		return;
	}

	//for (int i = 0; i < fileCount; i++) {
	//	strcpy_s(ListFileResp.Data.fileName[i], 255, "TEST FILE.PLACEHOLDER");
	//	ListFileResp.Data.fileSize[i] = 536870912;
	//}

	//ListFileResp.Data.currentPage = 1;
	//ListFileResp.Data.totalPage = 2;

	//sendData(hClientSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);

	return;
}