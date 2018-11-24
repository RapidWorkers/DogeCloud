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
	@author �۸۾ƾ߿��غ�
	@brief ����� ���ϼ��� �۾� ���� �Լ�
*/

#include "FileServer.h"

/**
	@fn void procListFile(SOCKET hClientSock)
	@brief ���� ����Ʈ ����
	@author �۸۾ƾ߿��غ�
	@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procListFile(SOCKET hClientSock) {
	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	recvData(hClientSock, ListFile.buf + 4, sizeof(cf_ListFile) - 4, 0);
	
	//ȣ��Ʈ Ư�� ����� ��ȯ
	ListFile.Data.opCode = OP_CF_LISTFILE;
	ListFile.Data.dataLen = ntohl(ListFile.Data.opCode);


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

	//���� ���� Ȯ��
	char *getFileCountQuery = "SELECT count(id) FROM fileList WHERE owner = ?";

	//���� ���δ� �غ�
	MYSQL_BIND bind_getFileCount_set[1];
	memset(bind_getFileCount_set, 0, sizeof(bind_getFileCount_set));
	bind_getFileCount_set[0].buffer_type = MYSQL_TYPE_LONG;
	bind_getFileCount_set[0].buffer = &userUID;
	bind_getFileCount_set[0].buffer_length = 4;

	//��� ���δ� �غ�
	int fileCount = 0;
	MYSQL_BIND bind_getFileCount_res[1];
	memset(&bind_getFileCount_res, 0, sizeof(bind_getFileCount_res)); //init bind
	bind_getFileCount_res[0].buffer_type = MYSQL_TYPE_LONG;
	bind_getFileCount_res[0].buffer = &fileCount;
	bind_getFileCount_res[0].buffer_length = 4;

	//MySQL Prepared Statement �ʱ�ȭ
	MYSQL_STMT *stmt;
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, errorLevel, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL ����
	sqlPrepareAndExecute(&sqlHandle, stmt, getFileCountQuery, bind_getFileCount_set, bind_getFileCount_res);

	//����� ��������
	if (mysql_stmt_fetch(stmt)) {
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: Got no data from Database!!!");
		printDebugMsg(DC_ERROR, errorLevel, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	mysql_stmt_free_result(stmt);

	//���� ���� ��Ŷ�� ����
	ListFileResp.Data.fileCount = fileCount;

	int totalPage = (fileCount - 1) / 10 + 1;
	ListFileResp.Data.totalPage = totalPage;

	if (page > totalPage) page = totalPage;
	else if (page < 1) page = 1;

	ListFileResp.Data.currentPage = page;

	if(fileCount == 0){//���� ������ 0����

		sendData(hClientSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);
		return;
	}

	//���� ��� ������ �о����
	char *getFileListQuery = "SELECT origName, fileSize FROM fileList WHERE owner = ? LIMIT ? , 10;";

	//���� ���δ� �غ�
	int startIdx = (page - 1) * 10;
	MYSQL_BIND bind_getFileList_set[2];
	memset(bind_getFileList_set, 0, sizeof(bind_getFileList_set));
	bind_getFileList_set[0].buffer_type = MYSQL_TYPE_LONG;
	bind_getFileList_set[0].buffer = &userUID;
	bind_getFileList_set[0].buffer_length = 4;
	bind_getFileList_set[1].buffer_type = MYSQL_TYPE_LONG;
	bind_getFileList_set[1].buffer = &startIdx;
	bind_getFileList_set[1].buffer_length = 4;

	//��� ���δ� �غ�
	char fileName[255];
	unsigned long fileSize;
	MYSQL_BIND bind_getFileList_res[2];
	memset(&bind_getFileList_res, 0, sizeof(bind_getFileList_res)); //init bind
	bind_getFileList_res[0].buffer_type = MYSQL_TYPE_STRING;
	bind_getFileList_res[0].buffer = fileName;
	bind_getFileList_res[0].buffer_length = 255;
	bind_getFileList_res[1].buffer_type = MYSQL_TYPE_LONG;
	bind_getFileList_res[1].buffer = &fileSize;
	bind_getFileList_res[1].buffer_length = 4;

	//MySQL Prepared Statement �ʱ�ȭ
	if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
		printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
		printDebugMsg(DC_ERROR, errorLevel, "MySQL Error: %s", mysql_stmt_error(stmt));
		printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
		system("pause");
		exit(1);//exit with error
	}

	//SQL ����
	sqlPrepareAndExecute(&sqlHandle, stmt, getFileListQuery, bind_getFileList_set, bind_getFileList_res);

	int i = 0;
	int fetch = 0;
	do {
		fetch = mysql_stmt_fetch(stmt);
		if (fetch == 1) {//�������
			printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: SQL Fetching Data Fail!!!");
			printDebugMsg(DC_ERROR, errorLevel, "MySQL Error: %s", mysql_stmt_error(stmt));
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
		}

		strcpy_s(ListFileResp.Data.fileName[i], 255, fileName);
		ListFileResp.Data.fileSize[i] = fileSize;
		i++;
	} while (fetch != MYSQL_NO_DATA);

	//��Ŷ ����
	sendData(hClientSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);
	return;
}

/**
@fn void procListFile(SOCKET hClientSock)
@brief ���� ���ٿ� ó��
@author �۸۾ƾ߿��غ�
@param hClientSock Ŭ���̾�Ʈ ����� ����
*/
void procFileJob(SOCKET hClientSock) {
	//��Ŷ ���� �� �ʱ�ȭ
	cf_FileJobReq FileJobReq;
	cffc_FileInfo FileInfo;
	fc_FileJobResult FileJobResult;
	memset(&FileJobReq, 0, sizeof(cf_FileJobReq));
	memset(&FileInfo, 0, sizeof(cffc_FileInfo));
	memset(&FileJobResult, 0, sizeof(fc_FileJobResult));

	//���� ��Ŷ ����
	recvData(hClientSock, FileJobReq.buf + 4, sizeof(cf_FileJobReq) - 4, 0);

	//ȣ��Ʈ Ư�� ����� ��ȯ
	FileJobReq.Data.opCode = OP_CF_FILEJOBREQ;
	FileJobReq.Data.dataLen = ntohl(FileJobReq.Data.dataLen);

	if (FileJobReq.Data.jobType == 0) {//(Ŭ���̾�Ʈ ��������)���ε�
		unsigned char fileHash[32];
		FILE *downFile;
		char fileName[255] = { 0, };
		char tmpFileName[65] = { 0, };
		unsigned char tmpRandomNum[16] = { 0, };
		unsigned char tmpHash[32] = { 0, };
		int userUID = 0;

		//�ӽ÷� ����� �����̸� ����
		GenerateCSPRNG(tmpRandomNum, 15);
		SHA256_Text(tmpRandomNum, tmpHash);
		for (int i = 0; i < 32; i++)
			sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

		//���ϳ��� ����
		sprintf_s(fileName, 255, "./storage/%s", tmpFileName);

		//���� ����
		if (fopen_s(&downFile, fileName, "wb+")) {
			printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: CANNOT Create TEMP FILE!!!");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}

		//�ٿ�ε� ��� ����
		unsigned long fileSize = 0;

		//���� ������ ����
		recvData(hClientSock, (char*)&fileSize, 4, 0);
		fileSize = ntohl(fileSize);
		unsigned int left = fileSize;
		unsigned int toRead;

		unsigned char dataBuffer[4096]; //4KiB
		while (1) {
			if (left < 4096U)
				toRead = left;
			else
				toRead = 4096U;

			recvData(hClientSock, dataBuffer, toRead, 0);
			fwrite(dataBuffer, toRead, 1, downFile);
			left -= toRead;
			if (!left) break;//�Ϸ�� Ż��
		}
		//�ٿ�ε� ��� ��

		//�ٿ�ε� ���� �ؽ� ���ϱ�
		getFileHash(downFile, fileHash);

		fclose(downFile);//���� �ݱ�

		//���� ���� ����
		recvData(hClientSock, FileInfo.buf, sizeof(cffc_FileInfo), 0);

		//ȣ��Ʈ Ư�� ��������� ��ȯ
		FileInfo.Data.opCode = ntohl(FileInfo.Data.opCode);
		FileInfo.Data.dataLen = ntohl(FileInfo.Data.dataLen);
		FileInfo.Data.fileSize = ntohl(FileInfo.Data.fileSize);

		//������Ŷ ����
		FileJobResult.Data.opCode = htonl(OP_FC_FILEJOBRESULT);
		FileJobResult.Data.dataLen = htonl(sizeof(fc_FileJobResult) - 8);
		
		if (memcmp(FileInfo.Data.encFileHash, fileHash, 32)) {//���� �ؽ� ����ġ
			remove(fileName);//�׳� ����
			FileJobResult.Data.statusCode = 0;
			//���� ����
			sendData(hClientSock, FileJobResult.buf, sizeof(fc_FileJobResult), 0);
			return;
		}
		
		//�������� ����
		FileJobResult.Data.statusCode = 1;

		//������������ uid ��������
		WaitForSingleObject(hMutex, INFINITE);
		for (int j = 0; j < clientCount; j++) {
			if (hClientSock == hClientSocks[j]) {
				userUID = sessionList[j].userUID;
				break;
			}
		}
		ReleaseMutex(hMutex);

		//���� ���� Ȯ��
		char *insertFileQuery = "INSERT into fileList VALUES(NULL, ?, ?, ?, ?, ?, ?);";

		//���� ���δ� �غ�
		MYSQL_BIND bind_insertFile_set[6];
		memset(bind_insertFile_set, 0, sizeof(bind_insertFile_set));
		bind_insertFile_set[0].buffer_type = MYSQL_TYPE_LONG;
		bind_insertFile_set[0].buffer = &userUID;
		bind_insertFile_set[0].buffer_length = 4;
		bind_insertFile_set[1].buffer_type = MYSQL_TYPE_LONG;
		bind_insertFile_set[1].buffer = &FileInfo.Data.fileSize;
		bind_insertFile_set[1].buffer_length = 4;
		bind_insertFile_set[2].buffer_type = MYSQL_TYPE_BLOB;
		bind_insertFile_set[2].buffer = FileInfo.Data.IV;
		bind_insertFile_set[2].buffer_length = 16;
		bind_insertFile_set[3].buffer_type = MYSQL_TYPE_STRING;
		bind_insertFile_set[3].buffer = tmpFileName;
		bind_insertFile_set[3].buffer_length = 65;
		bind_insertFile_set[4].buffer_type = MYSQL_TYPE_STRING;
		bind_insertFile_set[4].buffer = FileInfo.Data.fileName;
		bind_insertFile_set[4].buffer_length = 255;
		bind_insertFile_set[5].buffer_type = MYSQL_TYPE_BLOB;
		bind_insertFile_set[5].buffer = FileInfo.Data.orgFileHash;
		bind_insertFile_set[5].buffer_length = 32;

		//MySQL Prepared Statement �ʱ�ȭ
		MYSQL_STMT *stmt;
		if ((stmt = mysql_stmt_init(&sqlHandle)) == NULL) {//stmt is local variable so this must be done before calling sqlPrepareAndExecute
			printDebugMsg(DC_ERROR, errorLevel, "FATAL ERROR: SQL Prepared Statement Initialize fail!!!");
			printDebugMsg(DC_ERROR, errorLevel, "MySQL Error: %s", mysql_stmt_error(stmt));
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);//exit with error
		}

		//SQL ����
		sqlPrepareAndExecute(&sqlHandle, stmt, insertFileQuery, bind_insertFile_set, NULL);

		sendData(hClientSock, FileJobResult.buf, sizeof(fc_FileJobResult), 0);
		return;
		
	}
	else if (FileJobReq.Data.jobType == 1) {//�ٿ�ε�

	}
	else {//��ȿ���� ����
		printDebugMsg(DC_WARN, errorLevel, "Invalid Job Type");
		return;
	}

}