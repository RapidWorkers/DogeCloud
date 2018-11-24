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
	@file fileServerConnector.c
	@date 2018/11/21
	@author �۸۾ƾ߿��غ�
	@brief DogeCloud ���ϼ��� ����� �Լ� ����
*/

#include "DogeCloud.h"

/**
	@fn void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey)
	@brief DogeCloud ���ϼ��� ���� �� ����� �޴� ���
	@author �۸۾ƾ߿��غ�
	@param *fileServerAddr ���� ���� �ּ�
	@param fileServerPort ���� ���� ��Ʈ
	@param *authKey ���� ���� ����Ű
*/
void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey) {

	/** @brief ���� ���� �ּ� �����ϴ� ����ü */
	SOCKADDR_IN fileSrvConAddr;

	/** @brief ���ϼ��� ���� */
	SOCKET hFileSrvSock;
	memset(&fileSrvConAddr, 0, sizeof(fileSrvConAddr));

	//��Ŷ ������ �޸� �Ҵ� �� 0���� �ʱ�ȭ
	cf_LoginFile LoginFile;
	fc_LoginFileResp LoginFileResp;
	memset(&LoginFile, 0, sizeof(cf_LoginFile));//0���� �ʱ�ȭ
	memset(&LoginFileResp, 0, sizeof(fc_LoginFileResp));

	//���ϼ��� �ּ� ����ü ����
	fileSrvConAddr.sin_family = AF_INET;
	fileSrvConAddr.sin_port = htons(fileServerPort);
	inet_pton(AF_INET, fileServerAddr, &fileSrvConAddr.sin_addr.s_addr);

	//���� ����
	hFileSrvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hFileSrvSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, errorLevel, "Invalid File Server Socket");
		system("pause");
		return;
	}

	//���ϼ��� ����
	int err = (connect(hFileSrvSock, (SOCKADDR*)&fileSrvConAddr, sizeof(fileSrvConAddr)) == SOCKET_ERROR);
	if (err) //������ �ִٸ�
	{
		printDebugMsg(DC_ERROR, errorLevel, "���ϼ��� ���� ����: %d", WSAGetLastError());
		system("pause");
		return;
	}

	printDebugMsg(DC_INFO, errorLevel, "���������� ���ϼ����� ����Ǿ����ϴ�.");
	Sleep(500);

	//���ϼ��� ���� ����
	printDebugMsg(DC_INFO, errorLevel, "���ϼ��� ���� ����.");

	//���ϼ��� �α��� ��Ŷ ����
	memcpy(LoginFile.Data.UserFileServerAuthKey, authKey, 32);//����Ű ��Ŷ���� ����
	LoginFile.Data.opCode = htonl(OP_CF_LOGINFILE);
	LoginFile.Data.dataLen = htonl(sizeof(cf_LoginFile) - 8);

	//���ϼ����� �α��� ��Ŷ ����
	sendData(hFileSrvSock, LoginFile.buf, sizeof(cf_LoginFile), 0);

	//�α��� ���� ������
	recvData(hFileSrvSock, LoginFileResp.buf, sizeof(fc_LoginFileResp), 0);

	//��Ŷ�� �ִ� ���ڸ� ȣ��Ʈ�� ������ �ε������ ����
	LoginFileResp.Data.opCode = ntohl(LoginFileResp.Data.opCode);
	LoginFileResp.Data.dataLen = ntohl(LoginFileResp.Data.dataLen);

	if (LoginFileResp.Data.statusCode == 0) {//������ ������ ��� ���� ���� �Ǵ� ���� ����
		printDebugMsg(DC_ERROR, errorLevel, "���ϼ��� ���� ���� ����");
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
		return;
	}

	//���� ���� �޽��� ���
	printDebugMsg(DC_INFO, errorLevel, "���ϼ��� ���� ����.");
	Sleep(500);

	//���ϼ��� �޴� ���
	doFileManage(hFileSrvSock);

	//���ϼ������� �۾��� ����Ǿ��� ���
	if (closesocket(hFileSrvSock)) {//���� ����
		printDebugMsg(DC_ERROR, errorLevel, "���ϼ��� ���� ���� ����: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
		system("pause");
		exit(1);
		return;
	}

	return;
}

/**
	@fn void doFileManage(SOCKET hFileSrvSock)
	@brief DogeCloud ���ϼ��� �޴� ���
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
*/
void doFileManage(SOCKET hFileSrvSock) {

	/** @brief ���� ���� �ּ� �����ϴ� ����ü */

	/** @brief ���� ������ ���� */
	unsigned char page = 1; //ó�� �������� 1������

	while (1) {//�޴� ����� ������ �� ���� ���ѹݺ�

		/** @brief ���� ������ */
		int errorFlag = 0;

		int fileCount = 0;
		unsigned char maxPage = 0;

		system("cls");

		showFileList(hFileSrvSock, &fileCount, &page, &maxPage, &errorFlag);//���� ����Ʈ ���

		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. ���ε�");
		printf_s("\n\t2. �ٿ�ε�");
		printf_s("\n\t3. ���� ����");
		printf_s("\n\t4. ���� ������");
		printf_s("\n\t5. ���� ������");
		printf_s("\n\t6. ������");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");

		//�޴� ����
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://���ε�
			doFileJob(hFileSrvSock, 0, fileCount, &errorFlag);
			break;
		case 2://�ٿ�ε�
			doFileJob(hFileSrvSock, 1, fileCount, &errorFlag);
			break;
		case 3://���� ����
			deleteFile(hFileSrvSock, fileCount, &errorFlag);
			break;
		case 4://���� ������
			if (page > 1) page--;
			break;
		case 5://���� ������
			if (maxPage > page) page++;
			break;
		case 6://������
			return;//�� �Լ� ����� ��ӵǴ� �ڵ忡�� ���� �����
			break;
		default: //��ȿ���� ���� �Է�
			printDebugMsg(DC_WARN, errorLevel, "�ùٸ��� ���� �Է��Դϴ�.");
			Sleep(1000);
			break;
		}

		if (errorFlag) {//������ �����Ǿ��� ���
			printDebugMsg(DC_ERROR, errorLevel, "���ϼ��� ��� ����");
			printDebugMsg(DC_ERROR, errorLevel, "���ϼ��� ������ �����մϴ�.");
			system("pause");
			return;//���ϼ��� ���� ����
		}
	}
}

/**
	@fn void showFileList(SOCKET hFileSrvSock, int *fileCount, unsigned char *page, unsigned char *maxPage, int *errorFlag)
	@brief DogeCloud ���� ��� �����ֱ�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param *page ���� ������ ����
	@param *fileCount ���� ���� ���� ����
	@param *maxPage ��ü ������ �� ���� ����
	@param *errorFlag ���� �÷���
*/
void showFileList(SOCKET hFileSrvSock, int *fileCount, unsigned char *page, unsigned char *maxPage, int *errorFlag) {
	//��Ŷ ������ �޸� �Ҵ� �� �ʱ�ȭ
	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));//0���� ����
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	//��Ŷ ����
	ListFile.Data.opCode = htonl(OP_CF_LISTFILE);
	ListFile.Data.dataLen = htonl(sizeof(cf_ListFile) - 8);
	ListFile.Data.page = *page;

	*errorFlag = 1;//�ϴ� errorFlag = 1�� ����
	//��Ŷ ����
	sendData(hFileSrvSock, ListFile.buf, sizeof(cf_ListFile), 0);
	//���� ������
	recvData(hFileSrvSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);
	*errorFlag = 0;//�� �Լ����� ���������� ����Ǹ� ��� �Լ��� ����ǹǷ� �ٽ� 0���� ����

	//�޾ƿ� ���ڸ� ȣ��Ʈ Ư�� �ε������ ����
	ListFileResp.Data.opCode = ntohl(ListFileResp.Data.opCode);
	ListFileResp.Data.dataLen = ntohl(ListFileResp.Data.dataLen);

	if (!ListFileResp.Data.statusCode) {//���н�
		*errorFlag = 1;//���� ����� �ҷ����� �������Ƿ�
		printDebugMsg(DC_ERROR, errorLevel, "���ϼ������� ���� ����� �������� ���߽��ϴ�.");
		return;
	}

	puts("\n******************************************************************************************");
	if (!ListFileResp.Data.fileCount) {//������ ���� ���
		printf_s("\n\t ������ �����ϴ�.");
	}
	else {//������� ������
		printf_s("\n%4s %30s %30s", "����", "���ϸ�", "���� �뷮");
		for (int i = 0; i < ListFileResp.Data.fileCount; i++) {//���ϸ�� ���
			char fileSize[32] = { 0, };
			minimizeFileSize(ListFileResp.Data.fileSize[i], fileSize);
			printf_s("\n%4d %30s %30s", i + 1, ListFileResp.Data.fileName[i], fileSize);
		}
	}
	printf_s("\n\t\t������ %d / %d", ListFileResp.Data.currentPage, ListFileResp.Data.totalPage);//������ ���
	puts("\n******************************************************************************************");

	*fileCount = ListFileResp.Data.fileCount;
	*maxPage = ListFileResp.Data.totalPage;
	*page = ListFileResp.Data.currentPage;

	return;
}

/**
	@fn void doFileJob(SOCKET hFileSrvSock, int jobType, int fileCount, int *errorFlag)
	@brief DogeCloud ���� ��/�ٿ�ε�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param jobType 0= ���ε�, 1= �ٿ�ε�
	@param fileCount ���� ���� ����
	@param *errorFlag ���� �÷���
*/
void doFileJob(SOCKET hFileSrvSock, int jobType, int fileCount, int *errorFlag) {
	//��Ŷ ���� �� �ʱ�ȭ
	cf_FileJobReq FileJobReq;
	cffc_FileInfo FileInfo;
	fc_FileJobResult FileJobResult;
	memset(&FileJobReq, 0, sizeof(cf_FileJobReq));
	memset(&FileInfo, 0, sizeof(cffc_FileInfo));
	memset(&FileJobResult, 0, sizeof(fc_FileJobResult));

	/** @brief �ӽ÷� ������ ������ ���� ����ü ������ */
	FILE *filePtr;

	/** @brief ���� �̸� */
	char fileName[255] = { 0, };

	/** @brief ���� ���� �ؽ� */
	char orgFileHash[32] = { 0, };

	/** @brief ��ȣȭ ���� �ؽ� */
	char encFileHash[32] = { 0, };

	/** @brief ���� ������ */
	unsigned long fileSize = 0;

	/** @brief ���� ��й�ȣ */
	char password[100] = { 0, };

	/** @brief ��ȣȭ�� ���� �ؽ��� ��ȯ�� ��й�ȣ */
	char pwdHash[32] = { 0, };

	/** @brief �ӽ÷� ������ ���� */
	unsigned char tmpRandomNum[16] = { 0, };

	/** @brief �ӽ÷� ������ ������ �ؽ� ����� */
	unsigned char tmpHash[32] = { 0, };

	/** @brief �Ϻ�ȣȭ�� ���� �ӽ÷� ������ ���� �̸� */
	char leaFileName[65] = { 0, };

	system("cls");

	if (jobType == 0) {//���ε�
		puts("���ε� �� ������ upload ������ ������ �� ���� �̸��� �Է��ϼ���.");
		printf_s("���� �̸�: ");
		scanf_s("%s", fileName, 255);

		if (strlen(fileName) > 220) {
			puts("���� �̸��� �ʹ� ��ϴ�. 220�� �̸����� �ٿ� �ּ���.");
			system("pause");
			return;
		}

		char tmpFileName[255];
		sprintf_s(tmpFileName, 255, "./upload/%s", fileName);

		if (fopen_s(&filePtr, tmpFileName, "rb")) {
			puts("�������� �ʴ� �����Դϴ�.");
			system("pause");
			return;
		}

		puts("������ ��ȣȭ�� ���� ��й�ȣ�� �Է����ּ���. �� �� �ؾ������ ���� �Ұ��մϴ�!!");
		printf_s("��й�ȣ: ");
		scanf_s("%99s", password, 100);

		//��й�ȣ �ؽ�
		SHA256_Text(password, pwdHash);

		//���� �ؽ� ���ϱ�
		puts("\n�ؽ� ������Դϴ�... ��ø� ��ٷ� �ּ���...");
		getFileHashProgress(filePtr, orgFileHash);
		puts("");
		
		
		//���� ���� ������ ���ϱ�
		fseek(filePtr, 0, SEEK_END);
		fileSize = ftell(filePtr);
		rewind(filePtr);

		//�ӽ÷� ����� �����̸� ����
		GenerateCSPRNG(tmpRandomNum, 15);
		SHA256_Text(tmpRandomNum, tmpHash);
		for (int i = 0; i < 32; i++)
			sprintf_s(leaFileName + (2 * i), 3, "%02x", tmpHash[i]);

		//��ȣȭ�� ���� �ӽ����� ����
		FILE *encTmpFile;
		if (fopen_s(&encTmpFile, leaFileName, "wb+")) {
			printDebugMsg(DC_ERROR, errorLevel, "������ ��������� �� �� �����ϴ�");
			printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
			system("pause");
			exit(1);
		}

		//IV ����
		unsigned char encIV[16] = { 0, };
		GenerateCSPRNG(encIV, 16);

		//��ȣȭ �ϱ�
		puts("��ȣȭ ó�����Դϴ�... ��ø� ��ٷ� �ּ���...");
		encryptFileLEAProgress(filePtr, encTmpFile, pwdHash, encIV);

		//��ȣȭ�� ������ �ؽ� ���ϱ�
		puts("\n�ؽ� ������Դϴ�... ��ø� ��ٷ� �ּ���...");
		getFileHashProgress(encTmpFile, encFileHash);
		puts("");

		//��û ��Ŷ ����
		FileJobReq.Data.opCode = htonl(OP_CF_FILEJOBREQ);
		FileJobReq.Data.dataLen = htonl(sizeof(cf_FileJobReq) - 8);
		FileJobReq.Data.jobType = 0;

		//��Ŷ ����
		sendData(hFileSrvSock, FileJobReq.buf, sizeof(cf_FileJobReq), 0);

		//���ε� ��� ����
		fseek(encTmpFile, 0, SEEK_END);
		/** @brief ���ε��� ���� ������ */
		unsigned long uploadFileSize = ftell(encTmpFile);
		/** @brief ���ε��� ���� �뷮 */
		unsigned long left = uploadFileSize;
		/** @brief �о�� ����Ʈ �� */
		unsigned int toRead;
		rewind(encTmpFile);

		//���� ������ ����
		uploadFileSize = htonl(fileSize);
		sendData(hFileSrvSock, (char*)&uploadFileSize, 4, 0);
		uploadFileSize = ntohl(uploadFileSize);

		/**
		@var unsigned char dataBuffer[4096]
		���ε� ���� 4KiB
		*/
		unsigned char dataBuffer[4096];

		puts("���� ���� ����");
		while (1) {//���ε� ������ ���� �ݺ�
			if (left < 4096U)//4KB���� ������ŭ ������
				toRead = left;//���� ��ŭ ����
			else//�ƴϸ�
				toRead = 4096U;//4KiB��ŭ ����

							   //���� �о ���ۿ� ����
			fread(dataBuffer, toRead, 1, encTmpFile);

			//�о�� ���۸� ������ ����
			if (!sendRaw(hFileSrvSock, dataBuffer, toRead, 0)) {
				printDebugMsg(DC_ERROR, errorLevel, "���� ����");
				printDebugMsg(DC_ERROR, errorLevel, "���α׷��� �����մϴ�.");
				system("pause");
				exit(1);
			}

			left -= toRead;//������ŭ ����
			updateProgress(uploadFileSize - left, uploadFileSize);//���α׷��� �� ������Ʈ(����)
			if (!left) break;//�Ϸ�� Ż��
		}

		puts("\n���� ���� �Ϸ�");

		//���ε� ��� ��

		//���� ���� ��Ŷ ����
		FileInfo.Data.opCode = htonl(OP_CFFC_FILEINFO);
		FileInfo.Data.dataLen = htonl(sizeof(cffc_FileInfo) - 8);
		FileInfo.Data.fileSize = htonl(fileSize);
		memcpy(FileInfo.Data.fileName, fileName, 255);
		memcpy(FileInfo.Data.orgFileHash, orgFileHash, 32);
		memcpy(FileInfo.Data.encFileHash, encFileHash, 32);
		memcpy(FileInfo.Data.IV, encIV, 16);

		//���� ���� ����
		sendData(hFileSrvSock, FileInfo.buf, sizeof(cffc_FileInfo), 0);

		//���� ��Ŷ ����
		recvData(hFileSrvSock, FileJobResult.buf, sizeof(fc_FileJobResult), 0);

		//ȣ��Ʈ Ư�� ��������� ����
		FileJobResult.Data.opCode = ntohl(FileJobResult.Data.opCode);
		FileJobResult.Data.dataLen = ntohl(FileJobResult.Data.dataLen);

		//�ӽ����� ����
		fclose(encTmpFile);
		fclose(filePtr);
		remove(leaFileName);

		//�������� �Ǵ�
		if (FileJobResult.Data.statusCode != 1) {
			puts("���� ������ �����Ͽ����ϴ�.");
			system("pause");
		}
		else {
			puts("���� ������ �����Ͽ����ϴ�.");
			system("pause");
		}
		return;
	
	}
	else if (jobType == 1) {//�ٿ�ε�

	}
	else {//�߸��� �Է��� �������� ����
		return;
	}
}

/**
	@fn void deleteFile(SOCKET hFileSrvSock, int fileCount, int *errorFlag)
	@brief DogeCloud ���� ����
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param fileCount ���� ���� ����
	@param *errorFlag ���� �÷���
*/
void deleteFile(SOCKET hFileSrvSock, int fileCount, int *errorFlag) {
	//��Ŷ ���� �� �ʱ�ȭ
	cf_DeleteFileReq DeleteFileReq;
	fc_DeleteFileResp DeleteFileResp;
	memset(&DeleteFileReq, 0, sizeof(DeleteFileReq));
	memset(&DeleteFileResp, 0, sizeof(DeleteFileResp));

	//�������� �Է� ����
	unsigned long fileID;
	printf_s("�� �� ������ �����Ͻðڽ��ϱ�? (���: 0): ");
	scanf_s("%u", &fileID);
	clearStdinBuffer();

	if (fileID == 0) return;
	if (fileID > fileCount || fileID < 0) {
		puts("��ȿ���� ���� �Է��Դϴ�.");
		system("pause");
		return;
	}

	unsigned char userInput;
	do {
		printf_s("���� %d�� ������ �����Ͻðڽ��ϱ�? (Y/N): ", fileID);
		scanf_s("%c", &userInput, 1);
		clearStdinBuffer();
	} while (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n');
	if (userInput == 'N' || userInput == 'n') return;
	//���� �Է� ��

	//��Ŷ ����
	DeleteFileReq.Data.opCode = htonl(OP_CF_DELETEFILEREQ);
	DeleteFileReq.Data.dataLen = htonl(sizeof(cf_DeleteFileReq));
	DeleteFileReq.Data.fileID = htonl(fileID);

	//��Ŷ ����
	sendData(hFileSrvSock, DeleteFileReq.buf, sizeof(cf_DeleteFileReq), 0);

	//���� �ޱ�
	recvData(hFileSrvSock, DeleteFileResp.buf, sizeof(fc_DeleteFileResp), 0);

	//ȣ��Ʈ Ư�� �ε������ ��ȯ
	DeleteFileResp.Data.opCode = ntohl(DeleteFileResp.Data.opCode);
	DeleteFileResp.Data.dataLen = ntohl(DeleteFileResp.Data.dataLen);

	if (DeleteFileResp.Data.statusCode != 1) {
		puts("���� ����!");
	}
	else {
		puts("���� ����!");
	}

	system("pause");
	return;
}