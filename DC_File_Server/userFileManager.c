#include "FileServer.h"

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