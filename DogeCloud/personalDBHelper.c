#include "DogeCloud.h"
#include <sqlite3.h>

void downloadPersonalDBFile(SOCKET hSocket) {
	cs_DownloadPersonalDBReq DownloadInfoReq;
	sc_DownloadPersonalDBResp DownloadInfoResp;
	memset(&DownloadInfoReq, 0, sizeof(cs_DownloadPersonalDBReq));
	memset(&DownloadInfoResp, 0, sizeof(sc_DownloadPersonalDBResp));

	DownloadInfoReq.Data.opCode = htonl(OP_CS_DOWNLOADUSERINFOREQ);
	DownloadInfoReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	sendData(hSocket, DownloadInfoReq.buf, sizeof(cs_DownloadPersonalDBReq), 0);
	//�ٿ�ε� ��� ����
	FILE *downFile;
	unsigned int toRead;

	if ((downFile = fopen("myinfoClient.db", "wb+")) == NULL) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "������ ��������� �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	unsigned long fileSize;
	recvRaw(hSocket, &fileSize, 4, 0);
	fileSize = ntohl(fileSize);
	unsigned long left = fileSize;

	puts("\n���� ���� �ٿ�ε� ����...");

	unsigned char dataBuffer[4096]; //4KiB

	while (1) {
		if (left < 4096U)
			toRead = left;
		else
			toRead = 4096U;

		if (!recvRaw(hSocket, dataBuffer, toRead, 0)) {
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���� ����");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
			system("pause");
			exit(1);
		}
		fwrite(dataBuffer, toRead, 1, downFile);
		left -= toRead;
		updateProgress(fileSize - left, fileSize);
		if (!left) break;//�Ϸ�� Ż��
	}

	puts("\n���� �Ϸ�");
	//�ٿ�ε� ��� ����
	recvData(hSocket, DownloadInfoResp.buf, sizeof(sc_DownloadPersonalDBResp), 0);
	DownloadInfoResp.Data.opCode = ntohl(DownloadInfoResp.Data.opCode);
	DownloadInfoResp.Data.dataLen = ntohl(DownloadInfoResp.Data.dataLen);

	unsigned char fileHash[32];
	getFileHash(downFile, fileHash);

	if (!memcmp(fileHash, DownloadInfoResp.Data.hash, 32)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "������ ����ġ");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	fclose(downFile);
	return;
}

void uploadPersonalDBFile(SOCKET hSocket, char* originalHash) {
	cs_PersonalDBEditDone PersonalDBEditDone;
	sc_PersonalDBEditDoneResp PersonalDBEditDoneResp;
	memset(&PersonalDBEditDone, 0, sizeof(cs_PersonalDBEditDone));
	memset(&PersonalDBEditDoneResp, 0, sizeof(sc_PersonalDBEditDoneResp));

	unsigned char fileHash[32];
	FILE *infoFile;

	if (fopen_s(&infoFile, "./myinfoClient.db", "rb")) {//�ؽ��� ���ϱ� ���ؼ�
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "������ �б������ �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	getFileHash(infoFile, fileHash);

	if (!memcmp(fileHash, originalHash, 32))//���࿡ ������ �������� �ʾҴٸ�
		return;//�׳� �����Ѵ�

	//������ �����̶�� ��� �����Ѵ�.

	PersonalDBEditDone.Data.opCode = htonl(OP_CS_PERSONALDBEDITDONE);
	PersonalDBEditDone.Data.dataLen = htonl(sizeof(cs_PersonalDBEditDone) - 8);
	memcpy(PersonalDBEditDone.Data.hash, fileHash, 32);

	int flag = 0;
	int count = 0;
	while (1) {
		puts("\n���� ���� ���ε� ����...");
		count++;
		sendData(hSocket, PersonalDBEditDone.buf, sizeof(cs_PersonalDBEditDone), 0);

		//���ε� ��� ����
		fseek(infoFile, 0, SEEK_END);
		unsigned int fileSize = ftell(infoFile);
		unsigned int left = fileSize;
		unsigned int toRead;
		fseek(infoFile, 0, SEEK_SET);

		//���� ������ ����
		sendData(hSocket, &fileSize, 4, 0);

		unsigned char dataBuffer[4096]; //4KiB
		while (1) {
			if (left < 4096U)
				toRead = left;
			else
				toRead = 4096U;

			fread(dataBuffer, toRead, 1, infoFile);

			if (!sendRaw(hSocket, dataBuffer, toRead, 0)) {
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���� ����");
				printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
				system("pause");
				exit(1);
			}

			left -= toRead;
			updateProgress(fileSize - left, fileSize);
			if (!left) break;//�Ϸ�� Ż��
		}

		//���ε� ��� ��
		recvData(hSocket, PersonalDBEditDoneResp.buf, sizeof(sc_PersonalDBEditDoneResp), 0);
		PersonalDBEditDoneResp.Data.opCode = ntohl(PersonalDBEditDoneResp.Data.opCode);
		PersonalDBEditDoneResp.Data.dataLen = ntohl(PersonalDBEditDoneResp.Data.dataLen);

		if (PersonalDBEditDoneResp.Data.statusCode)//������ Ż��
			break;

		if (!flag && count == 3) {//3�� �� ������ ���
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭���� ������ ������ �� �������ϴ�.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "myinfoClient.db ������ �������� ����� �ֽʽÿ�.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�� �� �����ڿ��� �̸��Ϸ� ������ �ֽø� �������� ����˴ϴ�.");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
			system("pause");
			exit(1);
		}
	}
	puts("\n���� ���� ���ε� �Ϸ�");
	system("pause");
	return;
}

void addContacts() {

};

void modifyContacts() {

}

void deleteContacts() {

}

void addMemo() {
	char tmpFileName[65] = { 0, };
	unsigned char tmpRandomNum[16] = { 0, };
	unsigned char tmpHash[32] = { 0, };
	char cmd[255] = { 0, };
	FILE *tmpFile;
	sqlite3 *dbHandle;
	char *insertMemo = "";

	//�ӽ÷� ����� �����̸� ����
	GenerateCSPRNG(tmpRandomNum, 15);
	SHA256_Text(tmpRandomNum, tmpHash);
	for (int i = 0; i < 32; i++)
		sprintf_s(tmpFileName + (2 * i), 3, "%02x", tmpHash[i]);

	//�ӽ����� ����
	if (fopen_s(&tmpFile, tmpFileName, "wb+")) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "������ ��������� �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}
	fclose(tmpFile);//������ ���� �ܺ� ���α׷��� �� �� �ֵ��� ���� ����


	sprintf_s(cmd, 255, "notepad %s", tmpFileName);
	puts("������ �Ϸ�Ǹ� �����ϰ� �޸����� �ݾ��ּ���.");
	system(cmd);

	if (fopen_s(&tmpFile, tmpFileName, "rb")) {//DB������ ���ؼ� �ٽ� ������ ����
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "������ �б������ �� �� �����ϴ�");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	if (sqlite3_open("myinfoClient.db", &dbHandle)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�����ͺ��̽� ������ ���� �� �����ϴ�.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
	}

	//TODO: db ó��

	//db ó�� �Ϸ�

	fclose(tmpFile);//���� ����
	remove(tmpFileName);//�ӽ����� ����
	sqlite3_close(dbHandle);//db���� �ݱ�

	return;
}

void modifyMemo() {

}

void deleteMemo() {

}