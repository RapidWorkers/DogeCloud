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
	//아래 코드는 테스트용으로 작성된 코드입니다.
	//반드시 수정이 필요합니다.

	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	recvData(hClientSock, ListFile.buf + 4, sizeof(cf_ListFile) - 4, 0);

	ListFileResp.Data.opCode = htonl(OP_FC_LISTFILERESP);
	ListFileResp.Data.dataLen = htonl(sizeof(fc_ListFileResp) - 8);
	ListFileResp.Data.statusCode = 1;

	ListFileResp.Data.fileCount = 8;
	for (int i = 0; i < 8; i++) {
		ListFileResp.Data.fileType[i] = 1;
		strcpy_s(ListFileResp.Data.fileName[i], 255, "TEST FILE.PLACEHOLDER");
	}

	ListFileResp.Data.currentPage = 1;
	ListFileResp.Data.totalPage = 2;

	strcpy_s(ListFileResp.Data.currentDir, 255, "/");

	sendData(hClientSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);

	return;
}