#include "RelayServer.h"
#include <sqlite3.h>

void procDownloadPersonalDBFile(SOCKET hClientSock) {
	cs_DownloadPersonalDBReq DownloadInfoReq;
	sc_DownloadPersonalDBResp DownloadInfoResp;
	memset(&DownloadInfoReq, 0, sizeof(cs_DownloadPersonalDBReq));
	memset(&DownloadInfoResp, 0, sizeof(sc_DownloadPersonalDBResp));

	recvData(hClientSock, DownloadInfoReq.buf + 4, sizeof(cs_DownloadPersonalDBReq) - 4, 0);

	DownloadInfoReq.Data.opCode = OP_CS_DOWNLOADUSERINFOREQ;
	DownloadInfoReq.Data.dataLen = ntohl(DownloadInfoReq.Data.dataLen);

	//업로드 모드 시작
	FILE *infoFile;
	unsigned int toRead;
	char fileName[255] = { 0, };

	WaitForSingleObject(hMutex, INFINITE);//to protect global var access
	for (int i = 0; i < clientCount; i++) {
		if (hClientSock == hClientSocks[i]) {
			sprintf_s(fileName, 255, "./infodb/%d.db", sessionList[i].userUID);
			break;
		}
	}
	ReleaseMutex(hMutex);

	//sqlite open으로 열어보기
	sqlite3 *dbHandle;
	if (sqlite3_open(fileName, &dbHandle)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "ERROR Reading Database File");
		sqlite3_close(dbHandle);
		return;
	}

	char *contactsDB = "CREATE TABLE IF NOT EXISTS contacts (\
		id integer PRIMARY KEY AUTOINCREMENT,\
		name varchar,\
		email varchar,\
		phone1 varchar,\
		phone2 varchar,\
		phone3 varchar\
		); ";

	char *memoDB = "CREATE TABLE IF NOT EXISTS memo (\
		id integer PRIMARY KEY AUTOINCREMENT,\
		user_text text\
		);";

	//만약 테이블이 존재하지 않으면 초기화시킴
	sqlite3_exec(dbHandle, contactsDB, 0, 0, 0);
	sqlite3_exec(dbHandle, memoDB, 0, 0, 0);
	sqlite3_close(dbHandle);

	unsigned long fileSize;
	if (fopen_s(&infoFile, fileName, "rb")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "ERROR Reading Database File");
		return;
	}

	fseek(infoFile, 0, SEEK_END);

	fileSize = ftell(infoFile);

	fileSize = htonl(fileSize);
	sendData(hClientSock, &fileSize, 4, 0);
	fileSize = ntohl(fileSize);

	fseek(infoFile, 0, SEEK_SET);

	unsigned long left = fileSize;

	unsigned char dataBuffer[4096]; //4KiB
	while (left) {
		if (left < 4096U)
			toRead = left;
		else
			toRead = 4096U;

		fread_s(dataBuffer, 4096, toRead, 1, infoFile);
		sendRaw(hClientSock, dataBuffer, toRead, 0);
		left -= toRead;
	}

	//Done Uploading
	DownloadInfoResp.Data.opCode = htonl(OP_SC_DOWNLOADUSERINFORESP);
	DownloadInfoResp.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	unsigned char fileHash[32];
	getFileHash(infoFile, fileHash);
	memcpy(DownloadInfoResp.Data.hash, fileHash, 32);

	sendData(hClientSock, DownloadInfoReq.buf, sizeof(sc_DownloadPersonalDBResp), 0);

	fclose(infoFile);
}

void procUploadPersonalDBFile(SOCKET hClientSock) {

}