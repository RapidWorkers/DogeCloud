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
	@file personalDBHandler.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief RelayServer ���� DB ���ٿ� �Լ� ����
*/

#include "RelayServer.h"
#include <sqlite3.h>

/**
	@fn void procDownloadPersonalDBFile(SOCKET hClientSock)
	@brief ������� ���� DB �ٿ�ε� ��û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����� ����� ����
*/
void procDownloadPersonalDBFile(SOCKET hClientSock) {
	cs_DownloadPersonalDBReq DownloadInfoReq;
	sc_DownloadPersonalDBResp DownloadInfoResp;
	memset(&DownloadInfoReq, 0, sizeof(cs_DownloadPersonalDBReq));
	memset(&DownloadInfoResp, 0, sizeof(sc_DownloadPersonalDBResp));

	recvData(hClientSock, DownloadInfoReq.buf + 4, sizeof(cs_DownloadPersonalDBReq) - 4, 0);

	DownloadInfoReq.Data.opCode = OP_CS_DOWNLOADUSERINFOREQ;
	DownloadInfoReq.Data.dataLen = ntohl(DownloadInfoReq.Data.dataLen);

	//���ε� ��� ����
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

	//sqlite open���� �����
	sqlite3 *dbHandle;
	if (sqlite3_open(fileName, &dbHandle)) {
		printDebugMsg(DC_ERROR, errorLevel, "ERROR Reading Database File");
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

	//���� ���̺��� �������� ������ �ʱ�ȭ��Ŵ
	sqlite3_exec(dbHandle, contactsDB, 0, 0, 0);
	sqlite3_exec(dbHandle, memoDB, 0, 0, 0);
	sqlite3_close(dbHandle);

	unsigned long fileSize;
	if (fopen_s(&infoFile, fileName, "rb")) {
		printDebugMsg(DC_ERROR, errorLevel, "ERROR Reading Database File");
		return;
	}

	fseek(infoFile, 0, SEEK_END);

	fileSize = ftell(infoFile);

	fileSize = htonl(fileSize);
	sendData(hClientSock, (char*)&fileSize, 4, 0);
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
		sendData(hClientSock, dataBuffer, toRead, 0);
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

/**
	@fn void procUploadPersonalDBFile(SOCKET hClientSock)
	@brief ������� ���� DB ���ε� ��û ó��
	@author �۸۾ƾ߿��غ�
	@param hClientSock ����� ����� ����
*/
void procUploadPersonalDBFile(SOCKET hClientSock) {
	cs_PersonalDBEditDone PersonalDBEditDone;
	sc_PersonalDBEditDoneResp PersonalDBEditDoneResp;
	memset(&PersonalDBEditDone, 0, sizeof(cs_PersonalDBEditDone));
	memset(&PersonalDBEditDoneResp, 0, sizeof(sc_PersonalDBEditDoneResp));

	unsigned char fileHash[32];
	FILE *infoFile;
	char fileName[255] = { 0, };
	char tmpFileName[65] = { 0, };
	unsigned char tmpRandomNum[16] = { 0, };
	unsigned char tmpHash[32] = { 0, };

	recvData(hClientSock, PersonalDBEditDone.buf + 4, sizeof(cs_PersonalDBEditDone) - 4, 0);

	PersonalDBEditDone.Data.opCode = ntohl(PersonalDBEditDone.Data.opCode);
	PersonalDBEditDone.Data.dataLen = ntohl(PersonalDBEditDone.Data.dataLen);

	//�ӽ÷� ����� �����̸� ����
	GenerateCSPRNG(tmpRandomNum, 15);
	SHA256_Text(tmpRandomNum, tmpHash);
	for (int i = 0; i < 32; i++)
		sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

	if (fopen_s(&infoFile, tmpFileName, "wb+")) {
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
		fwrite(dataBuffer, toRead, 1, infoFile);
		left -= toRead;
		if (!left) break;//�Ϸ�� Ż��
	}

	//���ε� ��� ��
	PersonalDBEditDoneResp.Data.opCode = ntohl(PersonalDBEditDoneResp.Data.opCode);
	PersonalDBEditDoneResp.Data.dataLen = ntohl(PersonalDBEditDoneResp.Data.dataLen);

	fseek(infoFile, 0, SEEK_SET);//ó������ �̵�
	getFileHash(infoFile, fileHash);
	fclose(infoFile);//���� ����

	if (!memcmp(fileHash, PersonalDBEditDone.Data.hash, 32)) {
		PersonalDBEditDoneResp.Data.statusCode = 1;//�ؽ� ��ġ
		WaitForSingleObject(hMutex, INFINITE);//to protect global var access
		for (int i = 0; i < clientCount; i++) {
			if (hClientSock == hClientSocks[i]) {
				sprintf_s(fileName, 255, "./infodb/%d.db", sessionList[i].userUID);
				break;
			}
		}
		ReleaseMutex(hMutex);
		remove(fileName);//����� ���ؼ� �� ������ ����
		rename(tmpFileName, fileName);//�ӽ������� db ������ �̵���Ŵ
	}
	else {
		PersonalDBEditDoneResp.Data.statusCode = 0;//�ؽ� ����ġ
		remove(tmpFileName);//���� ���� ������ ����
	}

	sendData(hClientSock, PersonalDBEditDoneResp.buf, sizeof(sc_PersonalDBEditDoneResp), 0);//��� ����
	return;
}