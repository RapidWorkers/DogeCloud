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
	@author �۸۾ƾ߿��غ�
	@brief ���� DB ���� �Լ� ����
*/

#include "DogeCloud.h"
#include <sqlite3.h>

/**
	@fn void downloadPersonalDBFile(SOCKET hSocket)
	@brief DogeCloud ���� DB ���� �ٿ�ε�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
*/
void downloadPersonalDBFile(SOCKET hSocket) {
	//��Ŷ ���� �Ҵ� �� �ʱ�ȭ
	cs_DownloadPersonalDBReq DownloadInfoReq;
	sc_DownloadPersonalDBResp DownloadInfoResp;
	memset(&DownloadInfoReq, 0, sizeof(cs_DownloadPersonalDBReq));//0���� �ʱ�ȭ
	memset(&DownloadInfoResp, 0, sizeof(sc_DownloadPersonalDBResp));

	/** @brief �ٿ�ε� ���� ������ ���� ����ü */
	FILE *downFile;

	/** @brief �޾ƿ� ���� ũ�� */
	unsigned int toWrite;

	/** @brief �޾ƿ� ���� ũ�� */
	unsigned long fileSize;

	/** @brief �ٿ�ε� ���� 4KiB */
	unsigned char dataBuffer[4096];

	/** @brief ���� �ؽ��� ����� */
	unsigned char fileHash[32];

	//��Ŷ ����
	DownloadInfoReq.Data.opCode = htonl(OP_CS_DOWNLOADUSERINFOREQ);
	DownloadInfoReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	//�ٿ�ε� ��û ��Ŷ ����
	sendData(hSocket, DownloadInfoReq.buf, sizeof(cs_DownloadPersonalDBReq), 0);

	//�ٿ�ε� ��� ����
	if (fopen_s(&downFile, "myinfoClient.db", "wb+")) {//DB ���� �ٿ�ε带 ���� ���� ����
		printDebugMsg(DC_ERROR, errorLevel, "������ ��������� �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	recvData(hSocket, (char*)&fileSize, 4, 0);
	fileSize = ntohl(fileSize);//ȣ��Ʈ Ư�� �ε������ ��ȯ

	/** @brief ���� ���� ũ�� */
	unsigned long left = fileSize;

	puts("\n���� ���� �ٿ�ε� ����...");

	while (1) {//���� ũ�Ⱑ 0�� �ɶ� ���� �ݺ�
		if (left < 4096U)//4KB���� ������ŭ ������
			toWrite = left;//���� ��ŭ �޾ƿ�
		else//�ƴϸ�
			toWrite = 4096U;//4KiB��ŭ �޾ƿ�

		if (!recvRaw(hSocket, dataBuffer, toWrite, 0)) {//�����κ��� �ٿ�ε�
			printDebugMsg(DC_ERROR, errorLevel, "���� ����");
			printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
			system("pause");
			exit(1);
		}

		fwrite(dataBuffer, toWrite, 1, downFile);//���Ͽ� �޾ƿ� ��ŭ �ۼ�
		left -= toWrite;//�޾ƿ� ��ŭ ����
		updateProgress(fileSize - left, fileSize);//���α׷��� �� ������Ʈ(����)
		if (!left) break;//�Ϸ�� Ż��
	}

	puts("\n���� �Ϸ�");
	//�ٿ�ε� ��� ����

	//�ٿ�ε� �Ϸ� ��Ŷ ������
	recvData(hSocket, DownloadInfoResp.buf, sizeof(sc_DownloadPersonalDBResp), 0);

	//ȣ��Ʈ �ε������ ��ȯ
	DownloadInfoResp.Data.opCode = ntohl(DownloadInfoResp.Data.opCode);
	DownloadInfoResp.Data.dataLen = ntohl(DownloadInfoResp.Data.dataLen);

	//�ٿ�ε� ���� ������ �ؽ� ���ϱ�
	getFileHash(downFile, fileHash);

	if (memcmp(fileHash, DownloadInfoResp.Data.hash, 32)) {//���� �ؽ����� �ٸ��ٸ�
		printDebugMsg(DC_ERROR, errorLevel, "������ ����ġ");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	fclose(downFile);//���� �ݱ�
	return;
}

/**
	@fn void uploadPersonalDBFile(SOCKET hSocket, char* originalHash)
	@brief DogeCloud ���� DB ���� ���ε�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰� ���� ����� ����
	@param *originalHash ���� �ؽ���
*/
void uploadPersonalDBFile(SOCKET hSocket, char* originalHash) {
	//��Ŷ ���� �Ҵ� �� �ʱ�ȭ
	cs_PersonalDBEditDone PersonalDBEditDone;
	sc_PersonalDBEditDoneResp PersonalDBEditDoneResp;
	memset(&PersonalDBEditDone, 0, sizeof(cs_PersonalDBEditDone));//0���� �ʱ�ȭ
	memset(&PersonalDBEditDoneResp, 0, sizeof(sc_PersonalDBEditDoneResp));

	/** @brief ���� �ؽ��� ����� */
	unsigned char fileHash[32];

	/** @brief DB ���� ���� ����ü */
	FILE *infoFile;

	//DB���� ����
	if (fopen_s(&infoFile, "./myinfoClient.db", "rb")) {
		printDebugMsg(DC_ERROR, errorLevel, "������ �б������ �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//�ؽ� ���ϱ�
	getFileHash(infoFile, fileHash);

	if (!memcmp(fileHash, originalHash, 32)) {//���࿡ ������ �������� �ʾҴٸ�
		fclose(infoFile);//���� �ݱ�
		return;//�׳� �����Ѵ�
	}

	//������ �����̶�� ��� �����Ѵ�.
	
	//��Ŷ ����
	PersonalDBEditDone.Data.opCode = htonl(OP_CS_PERSONALDBEDITDONE);
	PersonalDBEditDone.Data.dataLen = htonl(sizeof(cs_PersonalDBEditDone) - 8);
	memcpy(PersonalDBEditDone.Data.hash, fileHash, 32);

	/** @brief ���ε� ���� ���� �÷��� */
	int flag = 0;

	/** @brief ���ε� Ƚ�� ī��Ʈ */
	int count = 0;

	while (1) {//���ε� Ƚ�� ä�ﶧ ���� �ݺ���Ŵ
		puts("\n���� ���� ���ε� ����...");
		count++;//ī��Ʈ ����

		//��Ŷ ����
		sendData(hSocket, PersonalDBEditDone.buf, sizeof(cs_PersonalDBEditDone), 0);

		//���ε� ��� ����
		fseek(infoFile, 0, SEEK_END);

		/** @brief ���ε��� ���� ������ */
		unsigned int fileSize = ftell(infoFile);

		/** @brief ���ε��� ���� �뷮 */
		unsigned int left = fileSize;

		/** @brief �о�� ����Ʈ �� */
		unsigned int toRead;

		fseek(infoFile, 0, SEEK_SET);

		//���� ������ ����
		fileSize = htonl(fileSize);
		sendData(hSocket, (char*)&fileSize, 4, 0);
		fileSize = ntohl(fileSize);

		/**
			@var unsigned char dataBuffer[4096]
			���ε� ���� 4KiB
		*/
		unsigned char dataBuffer[4096];

		while (1) {//���ε� ������ ���� �ݺ�
			if (left < 4096U)//4KB���� ������ŭ ������
				toRead = left;//���� ��ŭ ����
			else//�ƴϸ�
				toRead = 4096U;//4KiB��ŭ ����

			//���� �о ���ۿ� ����
			fread(dataBuffer, toRead, 1, infoFile);

			//�о�� ���۸� ������ ����
			if (!sendRaw(hSocket, dataBuffer, toRead, 0)) {
				printDebugMsg(DC_ERROR, errorLevel, "���� ����");
				printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
				system("pause");
				exit(1);
			}

			left -= toRead;//������ŭ ����
			updateProgress(fileSize - left, fileSize);//���α׷��� �� ������Ʈ(����)
			if (!left) break;//�Ϸ�� Ż��
		}

		//���ε� ��� ��

		//���ε� �Ϸ� ��Ŷ ������
		recvData(hSocket, PersonalDBEditDoneResp.buf, sizeof(sc_PersonalDBEditDoneResp), 0);

		//���ڸ� ȣ��Ʈ Ư�� �ε������ ����
		PersonalDBEditDoneResp.Data.opCode = ntohl(PersonalDBEditDoneResp.Data.opCode);
		PersonalDBEditDoneResp.Data.dataLen = ntohl(PersonalDBEditDoneResp.Data.dataLen);

		if (PersonalDBEditDoneResp.Data.statusCode)//������ Ż��
			break;

		if (!flag && count == 3) {//3�� �� ������ ���
			printDebugMsg(DC_ERROR, errorLevel, "�߰輭���� ������ ������ �� �������ϴ�.");
			printDebugMsg(DC_ERROR, errorLevel, "myinfoClient.db ������ �������� ����� �ֽʽÿ�.");
			printDebugMsg(DC_ERROR, errorLevel, "�� �� �����ڿ��� �̸��Ϸ� ������ �ֽø� �������� ����˴ϴ�.");
			printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
			system("pause");
			exit(1);
		}
	}
	fclose(infoFile);
	puts("\n���� ���� ���ε� �Ϸ�");
	system("pause");
	return;
}

void addContacts() {
	DC_CONTACTS tmpContacts;
	memset(&tmpContacts, 0, sizeof(DC_CONTACTS));

	//����ó ����ڷκ��� �޾ƿ�
	printf_s("�̸�(�ִ� �ѱ� 15��, ���ĺ� �� ���� 30��, ��Ҵ� �ƹ��͵� �Է����� �ʰ� ����): ");
	scanf_s("%30[^\n]", tmpContacts.name, 31);
	clearStdinBuffer();
	if (strlen(tmpContacts.name) == 0) return; //�̸��� ������ ����

	printf_s("�̸���(�ִ� 50��) ");
	scanf_s("%50[^\n]", tmpContacts.email, 51);
	clearStdinBuffer();
	printf_s("����ó1(�ִ� 20��) ");
	scanf_s("%20[^\n]", tmpContacts.phone1, 21);
	clearStdinBuffer();
	printf_s("����ó2(�ִ� 20��) ");
	scanf_s("%20[^\n]", tmpContacts.phone2, 21);
	clearStdinBuffer();
	printf_s("����ó3(�ִ� 20��) ");
	scanf_s("%20[^\n]", tmpContacts.phone3, 21);
	clearStdinBuffer();

	/** @brief sqlite3 �ڵ� ����ü ������ */
	sqlite3 *dbHandle;

	/** @brief ����ó ������ ���� ���� */
	char *insertMemo = "INSERT into contacts VALUES(NULL, '%q', '%q', '%q', '%q', '%q');";

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB ����
		printDebugMsg(DC_ERROR, errorLevel, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	/** @brief �����ϱ� ���� �ϼ��� ���� */
	char* query = sqlite3_mprintf(insertMemo, tmpContacts.email, tmpContacts.name, tmpContacts.phone1, tmpContacts.phone2, tmpContacts.phone3);

	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//������ ���� => db ����
	sqlite3_close(dbHandle);//db���� �ݱ�

	//���� �Ҵ� ���� free
	sqlite3_free(query);

	return;
};

/**
@fn void modifyContacts(int count)
@brief DogeCloud DB�� ����ó ����
@author �۸۾ƾ߿��غ�
@param count ���� ����ó ����
*/
void modifyContacts(int count) {
	DC_CONTACTS tmpContacts;

	/** @brief ������Ʈ ���� */
	char *selectContacts = "SELECT * FROM contacts WHERE id = %d;";
	/** @brief ������Ʈ ���� */
	char *updateContacts = "UPDATE contacts SET name = '%q', email = '%q', phone1 = '%q', phone2 = '%q', phone3 = '%q' WHERE id = %d;";

	//�������� �Է� ����
	int contactsID;
	printf_s("�� �� ����ó�� �����Ͻðڽ��ϱ�? (���: 0): ");
	scanf_s("%d", &contactsID);
	clearStdinBuffer();

	if (contactsID == 0) return;
	if (contactsID < 0 || contactsID > count) {
		puts("��ȿ���� ���� �Է��Դϴ�.");
		system("pause");
		return;
	}

	/** @brief sqlite3�� ���� handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB ����
		printDebugMsg(DC_ERROR, errorLevel, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
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
		printf_s("\n %16s %30s %20s %20s %20s", "�̸�", "�̸���", "����ó1", "����ó2", "����ó3");
		printf_s("\n %16s %30s %20s %20s %20s", tmpContacts.name, tmpContacts.email, tmpContacts.phone1, tmpContacts.phone2, tmpContacts.phone3);
		printf_s("\n******************************************************************************************");
		puts("\n������ ���� ���� �� �ּ���.");
		puts("0: ���� 1:�̸� 2:�̸��� 3:����ó1 4:����ó2 5:����ó3");
		puts("\n******************************************************************************************");
		printf_s("=> ");
		scanf_s("%d", &userInput);
		clearStdinBuffer();

		char tmpName[31] = { 0, };
		
		switch (userInput) {
		case 0:
			break;
		case 1:
			printf_s("�̸�(�ִ� �ѱ� 15��, ���ĺ� �� ���� 30��, ��Ҵ� �ƹ��͵� �Է����� �ʰ� ����): ");
			scanf_s("%30[^\n]", tmpName, 31);
			clearStdinBuffer();
			if (!strlen(tmpName)) break;
			memcpy(tmpContacts.name, tmpName, 31);
			break;
		case 2:
			printf_s("�̸���(�ִ� 50��) ");
			scanf_s("%50[^\n]", tmpContacts.email, 51);
			clearStdinBuffer();
			break;
		case 3:
			printf_s("����ó1(�ִ� 20��) ");
			scanf_s("%20[^\n]", tmpContacts.phone1, 21);
			clearStdinBuffer();
			break;
		case 4:
			printf_s("����ó2(�ִ� 20��) ");
			scanf_s("%20[^\n]", tmpContacts.phone2, 21);
			clearStdinBuffer();
			break;
		case 5:
			printf_s("����ó3(�ִ� 20��) ");
			scanf_s("%20[^\n]", tmpContacts.phone3, 21);
			clearStdinBuffer();
			break;
		default:
			printDebugMsg(DC_WARN, errorLevel, "�ùٸ��� ���� �Է��Դϴ�.");
			Sleep(1000);
			break;
		}

	} while (userInput != 0);

	/** @brief �ϼ��� ���� ���� */
	char* query = sqlite3_mprintf(updateContacts, tmpContacts.name, tmpContacts.email, tmpContacts.phone1, tmpContacts.phone2, tmpContacts.phone3, contactsID);
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//����
	sqlite3_free(query);//�����Ҵ� free
	sqlite3_close(dbHandle);//db ����

	return;
}

/**
@fn void deleteContacts(int count)
@brief DogeCloud DB�� ����ó ����
@author �۸۾ƾ߿��غ�
@param count ���� ����ó ����
*/
void deleteContacts(int count) {

	/** @brief ������ ���� */
	char *deleteMemo = "DELETE FROM contacts WHERE id = %d;";
	/** @brief ���� �� �ε��� �� Ȯ�� ���� */
	char *selectIndex = "SELECT id FROM contacts LIMIT 1 OFFSET %d;";
	/** @brief �ε��� �缳�� ���� */
	char *updateIndex = "UPDATE contacts SET id = %d WHERE id = %d;";
	/** @brief auto increment �缳�� ���� */
	char *reIndex = "delete from sqlite_sequence where name='contacts';";

	//�������� �Է� ����
	int contactsID;
	printf_s("�� �� ����ó�� �����Ͻðڽ��ϱ�? (���: 0): ");
	scanf_s("%d", &contactsID);
	clearStdinBuffer();

	if (contactsID == 0) return;
	if (contactsID > count || contactsID < 0) {
		puts("��ȿ���� ���� �Է��Դϴ�.");
		system("pause");
		return;
	}

	unsigned char userInput;
	do {
		printf_s("���� %d�� ����ó�� �����Ͻðڽ��ϱ�? (Y/N): ", contactsID);
		scanf_s("%c", &userInput, 1);
		clearStdinBuffer();
	} while (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n');
	if (userInput == 'N' || userInput == 'n') return;
	//���� �Է� ��

	/** @brief sqlite3�� ���� handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB ����
		printDebugMsg(DC_ERROR, errorLevel, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	/** @brief �ϼ��� ���� ���� */
	char* query = sqlite3_mprintf(deleteMemo, contactsID);
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//����
	sqlite3_free(query);//�����Ҵ� free

	//�ε����� �ٽ� ����

	/** @brief sqlite3�� ���� stmt */
	sqlite3_stmt* stmt;
	for (int i = contactsID; i < count; i++) {//������ �κк��� ����
		char *selIdxReady = sqlite3_mprintf(selectIndex, i - 1);
		if (sqlite3_prepare(dbHandle, selIdxReady, -1, &stmt, NULL) == SQLITE_OK)
		{
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				/** @brief �ε��� ������Ʈ�� ���� �ϼ��� ���� */
				char *updateIdxReady = sqlite3_mprintf(updateIndex, i, sqlite3_column_int(stmt, 0));//�� ��ȣ = �ε��� ��
				sqlite3_exec(dbHandle, updateIdxReady, NULL, NULL, NULL);
				sqlite3_free(updateIdxReady);
			}
		}
		sqlite3_finalize(stmt);
		sqlite3_free(selIdxReady);//�����Ҵ� ����
	}
	sqlite3_exec(dbHandle, reIndex, NULL, NULL, NULL);//auto increment �� �ٽ� ����

	sqlite3_close(dbHandle);//db ����

	return;
}

/**
	@fn void addMemo()
	@brief DogeCloud DB�� �޸� �߰�
	@author �۸۾ƾ߿��غ�
*/
void addMemo() {
	/** @brief �ӽ÷� ������ ���� �̸� */
	char tmpFileName[65] = { 0, };

	/** @brief �ӽ÷� ������ ���� */
	unsigned char tmpRandomNum[16] = { 0, };

	/** @brief �ӽ÷� ������ ������ �ؽ� ����� */
	unsigned char tmpHash[32] = { 0, };

	/** @brief ������ Ŀ�ǵ� ����� */
	char cmd[255] = { 0, };

	/** @brief �ӽ÷� ������ ������ ���� ����ü ������ */
	FILE *tmpFile;

	/** @brief sqlite3 �ڵ� ����ü ������ */
	sqlite3 *dbHandle;

	/** @brief �޸� ������ ���� ���� */
	char *insertMemo = "INSERT into memo VALUES(NULL, '%q');";

	//�ӽ÷� ����� �����̸� ����
	GenerateCSPRNG(tmpRandomNum, 15);
	SHA256_Text(tmpRandomNum, tmpHash);
	for (int i = 0; i < 32; i++)
		sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

	//�ӽ����� ����
	if (fopen_s(&tmpFile, tmpFileName, "wb+")) {
		printDebugMsg(DC_ERROR, errorLevel, "������ ��������� �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}
	fclose(tmpFile);//������ ���� �ܺ� ���α׷��� �� �� �ֵ��� ���� ����

	//�޸��� ����
	sprintf_s(cmd, 255, "notepad %s", tmpFileName);
	puts("������ �Ϸ�Ǹ� �����ϰ� �޸����� �ݾ��ּ���.");
	system(cmd);

	if (fopen_s(&tmpFile, tmpFileName, "rb")) {//DB������ ���ؼ� �ٽ� ������ ����
		printDebugMsg(DC_ERROR, errorLevel, "������ �б������ �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB ����
		printDebugMsg(DC_ERROR, errorLevel, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	/** @brief �޸� ���� ������ */
	unsigned long memoFileSize;
	fseek(tmpFile, 0, SEEK_END);
	memoFileSize = ftell(tmpFile);

	if (memoFileSize == 0) {//���� ���� ����� 0�̸� �ƹ� �۾��� ���°�
		sqlite3_close(dbHandle);
		fclose(tmpFile);
		remove(tmpFileName);//�ӽ����� ����
		return;
	}

	rewind(tmpFile);//ó�� �κ����� �̵�

	//�������� �ֱ� ���� �޸� ũ�⸸ŭ �����Ҵ�
	char* text = (void*)calloc(1, memoFileSize+1);

	if (text == NULL) {
		printDebugMsg(DC_ERROR, errorLevel, "�޸� �Ҵ翡 �����߽��ϴ�.");
		return;
	}

	/** @brief ������ �о�;� �ϴ� ũ�� */
	unsigned long left = memoFileSize;
	/** @brief �󸶸�ŭ �о���� */
	unsigned long toRead;

	while (1) {//�� ���� �� ���� �ݺ�
		if (left < 4096U)//4KB���� ������ŭ ������
			toRead = left;//���� ��ŭ ����
		else//�ƴϸ�
			toRead = 4096U;//4KiB��ŭ ����
		//���� �о ���ۿ� ����
		fread(text + (memoFileSize - left), toRead, 1, tmpFile);

		left -= toRead;//������ŭ ����
		updateProgress(memoFileSize - left, memoFileSize);//���α׷��� �� ������Ʈ(����)
		if (!left) break;//�Ϸ�� Ż��
	}

	/** @brief �����ϱ� ���� �ϼ��� ���� */
	char* query = sqlite3_mprintf(insertMemo, text);
	
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//������ ���� => db ����

	fclose(tmpFile);//���� ����
	remove(tmpFileName);//�ӽ����� ����
	sqlite3_close(dbHandle);//db���� �ݱ�

	//���� �Ҵ� ���� free
	free(text);
	sqlite3_free(query);

	return;
}

/**
@fn void modifyMemo(int count)
@brief DogeCloud DB�� �޸� ����
@author �۸۾ƾ߿��غ�
@param count ���� �޸� ����
*/
void modifyMemo(int count) {
	/** @brief �ӽ÷� ������ ���� �̸� */
	char tmpFileName[65] = { 0, };

	/** @brief �ӽ÷� ������ ���� */
	unsigned char tmpRandomNum[16] = { 0, };

	/** @brief �ӽ÷� ������ ������ �ؽ� ����� */
	unsigned char tmpHash[32] = { 0, };

	/** @brief ������ Ŀ�ǵ� ����� */
	char cmd[255] = { 0, };

	/** @brief �ӽ÷� ������ ������ ���� ����ü ������ */
	FILE *tmpFile;

	/** @brief sqlite3 �ڵ� ����ü ������ */
	sqlite3 *dbHandle;

	/** @brief ������ �޸� ã�ƿ��� ���� */
	char *selectMemo = "SELECT user_text FROM memo WHERE id = %d;";
	/** @brief �޸� ������ ���� ���� */
	char *modifyMemo = "UPDATE memo SET user_text = '%q' WHERE id = %d;";

	//�������� �Է� ����
	int memoID;
	printf_s("�� �� �޸� �����Ͻðڽ��ϱ�? (���: 0): ");
	scanf_s("%d", &memoID);
	clearStdinBuffer();

	if (memoID == 0) return;
	else if (memoID > count || memoID < 0) {
		puts("��ȿ���� ���� �Է��Դϴ�.");
		system("pause");
		return;
	}
	//���� �Է� ��

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB ����
		printDebugMsg(DC_ERROR, errorLevel, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//�ӽ÷� ����� �����̸� ����
	GenerateCSPRNG(tmpRandomNum, 15);
	SHA256_Text(tmpRandomNum, tmpHash);
	for (int i = 0; i < 32; i++)
		sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

	//�ӽ����� ����
	if (fopen_s(&tmpFile, tmpFileName, "wb+")) {
		printDebugMsg(DC_ERROR, errorLevel, "������ ��������� �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
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

			/** @brief �󸶸�ŭ ���� */
			unsigned long toWrite;

			while (1) {//�� �� �� ���� �ݺ�
				if (left < 4096U)//4KB���� ������ŭ ������
					toWrite = left;//���� ��ŭ �ۼ�
				else//�ƴϸ�
					toWrite = 4096U;//4KiB��ŭ ����
				//���� �а� ���ۿ� ����
				fwrite(user_text + (orgMemoFileSize - left), toWrite, 1, tmpFile);

				left -= toWrite;//����ŭ ����
				updateProgress(orgMemoFileSize - left, orgMemoFileSize);//���α׷��� �� ������Ʈ(����)
				if (!left) break;//�Ϸ�� Ż��
			}
		}
	}
	sqlite3_finalize(stmt);
	sqlite3_free(selectQueryRdy);

	unsigned char orgHash[32] = { 0, };
	getFileHash(tmpFile, orgHash);

	fclose(tmpFile);//������ ���� �ܺ� ���α׷��� �� �� �ֵ��� ���� ����
	
	//�޸��� ����
	sprintf_s(cmd, 255, "notepad %s", tmpFileName);
	puts("\n������ �Ϸ�Ǹ� �����ϰ� �޸����� �ݾ��ּ���.");
	system(cmd);

	if (fopen_s(&tmpFile, tmpFileName, "rb")) {//DB�б⸦ ���ؼ� ������ ����
		printDebugMsg(DC_ERROR, errorLevel, "������ �б������ �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	/** @brief ������ �޸� ���� ������ */
	unsigned long newMemoFileSize;
	fseek(tmpFile, 0, SEEK_END);
	newMemoFileSize = ftell(tmpFile);

	unsigned char newHash[32];
	getFileHash(tmpFile, newHash);

	if (!memcmp(newHash, orgHash, 32)) {//���� ���� �ؽð� ���ٸ� �׳� ������
		sqlite3_close(dbHandle);
		fclose(tmpFile);
		remove(tmpFileName);//�ӽ����� ����
		return;
	}

	rewind(tmpFile);//ó�� �κ����� �̵�

	//�������� �ֱ� ���� �޸� ũ�⸸ŭ �����Ҵ�
	char* newText = (void*)calloc(1, newMemoFileSize + 1);

	if (newText == NULL) {
		printDebugMsg(DC_ERROR, errorLevel, "�޸� �Ҵ翡 �����߽��ϴ�.");
		return;
	}

	/** @brief ������ �о�;� �ϴ� ũ�� */
	unsigned long left = newMemoFileSize;
	/** @brief �󸶸�ŭ �о���� */
	unsigned long toRead;

	while (1) {//�� ���� �� ���� �ݺ�
		if (left < 4096U)//4KB���� ������ŭ ������
			toRead = left;//���� ��ŭ ����
		else//�ƴϸ�
			toRead = 4096U;//4KiB��ŭ ����
		//���� �о ���ۿ� ����
		fread(newText + (newMemoFileSize - left), toRead, 1, tmpFile);

		left -= toRead;//������ŭ ����
		updateProgress(newMemoFileSize - left, newMemoFileSize);//���α׷��� �� ������Ʈ(����)
		if (!left) break;//�Ϸ�� Ż��
	}

	/** @brief �����ϱ� ���� �ϼ��� ���� */
	char* updateQueryRdy = sqlite3_mprintf(modifyMemo, newText, memoID);

	sqlite3_exec(dbHandle, updateQueryRdy, NULL, NULL, NULL);//������ ���� => db ������Ʈ

	fclose(tmpFile);//���� ����
	remove(tmpFileName);//�ӽ����� ����
	sqlite3_close(dbHandle);//db���� �ݱ�

	//���� �Ҵ� ���� free
	free(newText);
	sqlite3_free(updateQueryRdy);

	return;
}

/**
@fn void deleteMemo(int count)
@brief DogeCloud DB�� �޸� ����
@author �۸۾ƾ߿��غ�
@param count ���� �޸� ����
*/
void deleteMemo(int count) {

	/** @brief ������ ���� */
	char *deleteMemo = "DELETE FROM memo WHERE id = %d;";
	/** @brief ���� �� �ε��� �� Ȯ�� ���� */
	char *selectIndex = "SELECT id FROM memo LIMIT 1 OFFSET %d;";
	/** @brief �ε��� �缳�� ���� */
	char *updateIndex = "UPDATE memo SET id = %d WHERE id = %d;";
	/** @brief auto increment �缳�� ���� */
	char *reIndex = "delete from sqlite_sequence where name='memo';";

	//�������� �Է� ����
	int memoID;
	printf_s("�� �� �޸� �����Ͻðڽ��ϱ�? (���: 0): ");
	scanf_s("%d", &memoID);
	clearStdinBuffer();

	if (memoID == 0) return;
	else if (memoID > count || memoID < 0) {
		puts("��ȿ���� ���� �Է��Դϴ�.");
		system("pause");
		return;
	}

	unsigned char userInput;
	do {
		printf_s("���� %d�� �޸� �����Ͻðڽ��ϱ�? (Y/N): ", memoID);
		scanf_s("%c", &userInput, 1);
		clearStdinBuffer();
	} while (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n');
	if (userInput == 'N' || userInput == 'n') return;
	//���� �Է� ��

	/** @brief sqlite3�� ���� handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB ����
		printDebugMsg(DC_ERROR, errorLevel, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	/** @brief �ϼ��� ���� ���� */
	char* query = sqlite3_mprintf(deleteMemo, memoID);
	sqlite3_exec(dbHandle, query, NULL, NULL, NULL);//����
	sqlite3_free(query);//�����Ҵ� free

	//�ε����� �ٽ� ����

	/** @brief sqlite3�� ���� stmt */
	sqlite3_stmt* stmt;
	for (int i = memoID; i < count; i++) {//������ �κк��� ����
		char *selIdxReady = sqlite3_mprintf(selectIndex, i - 1);
		if (sqlite3_prepare(dbHandle, selIdxReady, -1, &stmt, NULL) == SQLITE_OK)
		{
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				/** @brief �ε��� ������Ʈ�� ���� �ϼ��� ���� */
				char *updateIdxReady = sqlite3_mprintf(updateIndex, i, sqlite3_column_int(stmt, 0));//�� ��ȣ = �ε��� ��
				sqlite3_exec(dbHandle, updateIdxReady, NULL, NULL, NULL);
				sqlite3_free(updateIdxReady);
			}
		}
		sqlite3_finalize(stmt);
		sqlite3_free(selIdxReady);//�����Ҵ� ����
	}
	sqlite3_exec(dbHandle, reIndex, NULL, NULL, NULL);//auto increment �� �ٽ� ����

	sqlite3_close(dbHandle);//db ����

	return;
}