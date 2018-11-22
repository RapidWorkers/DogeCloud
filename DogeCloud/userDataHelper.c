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
	@author �۸۾ƾ߿��غ�
	@brief ����, ����ó, �޸� ���� �޴� ��¿� �Լ� ����
*/

#include "DogeCloud.h"
#include <sqlite3.h>

/**
	@fn void manageContacts(SOCKET hSocket)
	@brief DogeCloud ����ó ó�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰輭�� ����� ����
*/
void manageContacts(SOCKET hSocket) {
	/** @brief DB ���� �ؽ� ����� */
	char originalHash[32];

	/** @brief db �о���� ���� ���� ����ü ������ */
	FILE *fp;

	system("cls");

	//DB���� ����
	if (fopen_s(&fp, "./myinfoClient.db", "rb")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//�ؽ� ���ϱ�
	getFileHash(fp, originalHash);//���߿� ���� Ȯ���� ���� ���� ���� �ؽ��� ������

	//sqlite���� ���� ���� ���� ����
	fclose(fp); 

	/** @brief �ִ� ������ �� */
	int maxpage = 2;

	/** @brief ���� ������ */
	int page = 1;

	while (1) {//����� �� ���� �ݺ�
		system("cls");
		printf_s("\n******************************************************************************************");
		printf_s("\n%4s %16s %30s %15s %15s %15s %15s", "����", "�̸�", "�̸���", "����ó1", "����ó2", "����ó3", "����ó4");
		printf_s("\n%4d %16s %30s %15s %15s %15s %15s", 1, "ȫ�浿��", "myemail@myemailtest.com", "010-0000-0000", "010-0000-0000", "010-0000-0000", "010-0000-0000");
		printf_s("\t\t\t ������ %d / %d", page, maxpage);
		printf_s("\n******************************************************************************************\n");
		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. �߰�");
		printf_s("\n\t2. ����");
		printf_s("\n\t3. ���� ������");
		printf_s("\n\t4. ���� ������");
		printf_s("\n\t5. ������� ������ ���� �� ������");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");

		//�޴� ����
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://�߰�
			addContacts();
			break;
		case 2://����
			modifyContacts();
			break;
		case 3://���� ������
			if (maxpage > page) page++;
			break;
		case 4://���� ������
			if (page > 1) page--;
			break;
		case 5://������
			uploadPersonalDBFile(hSocket, originalHash);
			return;
			break;
		default: //��ȿ���� ���� �Է�
			printDebugMsg(DC_WARN, DC_ERRORLEVEL, "�ùٸ��� ���� �Է��Դϴ�.");
			Sleep(1000);
			break;
		}
	}
	return;
}

/**
	@fn void manageMemo(SOCKET hSocket)
	@brief DogeCloud �޸� ó�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰輭�� ����� ����
*/
void manageMemo(SOCKET hSocket) {
	/** @brief DB ���� �ؽ� ����� */
	unsigned char originalHash[32];

	/** @brief ���� �о���� ���� ����ü ������ */
	FILE *fp;

	system("cls");

	//DB���� ����
	if (fopen_s(&fp, "./myinfoClient.db", "rb")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	getFileHash(fp, originalHash);//���߿� ���� Ȯ���� ���� ���� ���� �ؽ��� ������

	//sqlite���� ���� ���� ���� ����
	fclose(fp);

	/** @brief sqlite3�� ���� handle */
	sqlite3 *dbHandle;
	if (sqlite3_open("myinfoClient.db", &dbHandle)) {//DB ����
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	char* countQuery = "SELECT count(id) FROM memo;";
	char* selectMemoQuery = "SELECT * FROM memo LIMIT 10 OFFSET %d;";

	/** @brief �޸� ���� */
	int count = 0;
	/** @brief ���� ������ */
	int page = 1;

	while (1) {//����� �� ���� �ݺ�

		sqlite3_stmt* stmt;
		if (sqlite3_prepare(dbHandle, countQuery, -1, &stmt, NULL) == SQLITE_OK)
		{
			if (sqlite3_step(stmt) == SQLITE_ROW) {
				count = sqlite3_column_int(stmt, 0);
			}
			else {
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
				system("pause");
				exit(1);
			}
		}
		sqlite3_finalize(stmt);

		/** @brief �ִ� ������ �� */
		int maxpage = (count-1) / 10 + 1;

		//�޸� ���
		system("cls");
		printf_s("\n******************************************************************************************");
		if (count == 0) {
			printf_s("\n%80s", "����� �޸� �����ϴ�.");
		}
		else {
			printf_s("\n%4s %60s", "����", "���� �Ϻ� (�ѱ� ���� �ִ� 30��)");
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
		printf_s("\n\t\t\t ������ %d / %d", page, maxpage);
		printf_s("\n******************************************************************************************\n");
		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. �߰�");
		printf_s("\n\t2. ���� �� ����");
		printf_s("\n\t3. ����");
		printf_s("\n\t4. ���� ������");
		printf_s("\n\t5. ���� ������");
		printf_s("\n\t6. ������� ������ ���� �� ������");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");

		//�޴� ����
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://�߰�
			addMemo();
			break;
		case 2://���� �� ����
			modifyMemo(count);
			break;
		case 3://����
			deleteMemo(count);
			break;
		case 4://���� ������
			if (page > 1) page--;
			break;
		case 5://���� ������
			if (maxpage > page) page++;
			break;
		case 6://������
			sqlite3_close(dbHandle);
			uploadPersonalDBFile(hSocket, originalHash);
			return;
			break;
		default: //��ȿ���� ���� �Է�
			printDebugMsg(DC_WARN, DC_ERRORLEVEL, "�ùٸ��� ���� �Է��Դϴ�.");
			Sleep(1000);
			break;
		}
	}
	return;
}


/**
	@fn void manageFile(SOCKET hSocket)
	@brief DogeCloud ���ϼ��� ó�� �Լ�
	@author �۸۾ƾ߿��غ�
	@param hSocket �߰輭�� ����� ����
*/
void manageFile(SOCKET hSocket) {

	//��Ŷ ���� �� �ʱ�ȭ
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	//��Ŷ ����
	FileSrvConReq.Data.opCode = htonl(OP_CS_FILESRVCONNREQ);
	FileSrvConReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	//��Ŷ ���� �� ����
	sendData(hSocket, FileSrvConReq.buf, sizeof(cs_FileSrvConReq), 0);
	recvData(hSocket, FileSrvConResp.buf, sizeof(sc_FileSrvConResp), 0);

	//ȣ��Ʈ �ε������ ����
	FileSrvConResp.Data.opCode = ntohl(FileSrvConResp.Data.opCode);
	FileSrvConResp.Data.dataLen = ntohl(FileSrvConResp.Data.dataLen);
	
	//���� �˻�
	if (!FileSrvConResp.Data.statusCode) {
		puts("���ϼ��� ������ ������ �� �����ϴ�!");
		puts("���� ������ ������ �ְų� ��� ó���� �����Ǵ� ���� �� �ֽ��ϴ�. ���߿� �ٽ� �õ����ּ���.");
		system("pause");
		return;
	}

	//���ϼ��� ����
	openFileServer(FileSrvConResp.Data.fileSrvAddr, FileSrvConResp.Data.fileSrvPort, FileSrvConResp.Data.authKey);
	return;
}