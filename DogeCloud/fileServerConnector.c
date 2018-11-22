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
	int currentPage = 1; //ó�� �������� 1������
	while (1) {//�޴� ����� ������ �� ���� ���ѹݺ�

		/** @brief ���� ������ */
		int errorFlag = 0;

		system("cls");
		showFileList(hFileSrvSock, &errorFlag);//���� ����Ʈ ���

		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. ���ε�");
		printf_s("\n\t2. �ٿ�ε�");
		printf_s("\n\t3. ���� ����");
		printf_s("\n\t4. ���� �̵�");
		printf_s("\n\t5. ���� ������");
		printf_s("\n\t6. ���� ������");
		printf_s("\n\t7. ������");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");

		//�޴� ����
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://���ε�
			doFileJob(hFileSrvSock, 0, &errorFlag);
			break;
		case 2://�ٿ�ε�
			doFileJob(hFileSrvSock, 1, &errorFlag);
			break;
		case 3://���� ����
			//deleteFile(hFileSrvSock);
			break;
		case 4://���� �̵�
			moveDir(hFileSrvSock, &errorFlag);
			break;
		case 5://���� �������� �̵�
			moveFileListPage(hFileSrvSock, 0, &errorFlag);
			break;
		case 6://���� �������� �̵�
			moveFileListPage(hFileSrvSock, 1, &errorFlag);
			break;
		case 7://������
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
	@fn void moveDir(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud ���ϼ��� ���丮 �̵�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param *errorFlag ���� �÷���
*/
void moveDir(SOCKET hFileSrvSock, int *errorFlag) {
	//��Ŷ ������ �޸� �Ҵ� �� �ʱ�ȭ
	cf_MoveDir MoveDir;
	fc_MoveDirResp MoveDirResp;
	memset(&MoveDir, 0, sizeof(cf_MoveDir));//0���� �ʱ�ȭ
	memset(&MoveDirResp, 0, sizeof(fc_MoveDirResp));

	//�������� �̵��� ���丮 �޾ƿ�
	char directory[255];
	printf_s("�̵��� ���丮 �Է�(�ִ� 254��) : ");
	scanf_s("%s", directory, 255);

	//���丮 �̵� ��Ŷ ����
	MoveDir.Data.opCode = htonl(OP_CF_MOVEDIR);
	MoveDir.Data.dataLen = htonl(sizeof(cf_MoveDir) - 8);
	strcpy_s(MoveDir.Data.moveDir, 255, directory);

	*errorFlag = 1;//�ϴ� errorFlag = 1�� ����
	//���丮 �̵� ��Ŷ ����
	sendData(hFileSrvSock, MoveDir.buf, sizeof(cf_MoveDir), 0);
	//���� �޾ƿ�
	recvData(hFileSrvSock, MoveDirResp.buf, sizeof(fc_MoveDirResp), 0);
	*errorFlag = 0;//�� �Լ����� ���������� ����Ǹ� ��� �Լ��� ����ǹǷ� �ٽ� 0���� ����

	//�޾ƿ� ���ڸ� ȣ��Ʈ Ư�� �ε������ ����
	MoveDirResp.Data.opCode = ntohl(MoveDirResp.Data.opCode);
	MoveDirResp.Data.dataLen = ntohl(MoveDirResp.Data.dataLen);

	if (!MoveDirResp.Data.statusCode) {//���丮 ���� ���н�
		printDebugMsg(DC_WARN, errorLevel, "�������� �ʴ� ���丮�̰ų� ��û�� �����߽��ϴ�.");
	}

	return;
}

/**
	@fn void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag)
	@brief DogeCloud ���� ��� ������ �̵�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param type 0= ���� ������, 1=���� ������
	@param *errorFlag ���� �÷���
*/
void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag) {
	//��Ŷ ������ �޸� �Ҵ� �� �ʱ�ȭ
	cf_ListPageMove ListPageMove;
	fc_ListPageMoveResp ListPageMoveResp;
	memset(&ListPageMove, 0, sizeof(cf_ListPageMove));//0���� �ʱ�ȭ
	memset(&ListPageMoveResp, 0, sizeof(fc_ListPageMoveResp));

	//������ �̵��� ��Ŷ ����
	ListPageMove.Data.opCode = htonl(OP_CF_LISTPAGEMOVE);
	ListPageMove.Data.dataLen = htonl(sizeof(cf_ListPageMove) - 8);
	ListPageMove.Data.moveTo = type;

	*errorFlag = 1;//�ϴ� errorFlag = 1�� ����
	//������ �̵� ��Ŷ ����
	sendData(hFileSrvSock, ListPageMove.buf, sizeof(cf_ListPageMove), 0);
	//���� ������
	recvData(hFileSrvSock, ListPageMoveResp.buf, sizeof(fc_ListPageMoveResp), 0);
	*errorFlag = 0;//�� �Լ����� ���������� ����Ǹ� ��� �Լ��� ����ǹǷ� �ٽ� 0���� ����

	//�޾ƿ� ���ڸ� ȣ��Ʈ Ư�� �ε������ ����
	ListPageMove.Data.opCode = ntohl(ListPageMove.Data.opCode);
	ListPageMove.Data.dataLen = ntohl(ListPageMove.Data.dataLen);

	if (!ListPageMoveResp.Data.statusCode) {//���н�
		*errorFlag = 1;
		printDebugMsg(DC_ERROR, errorLevel, "��û�� �����߽��ϴ�.");
	}

	return;
}

/**
	@fn void showFileList(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud ���� ��� �����ֱ�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param *errorFlag ���� �÷���
*/
void showFileList(SOCKET hFileSrvSock, int *errorFlag) {
	//��Ŷ ������ �޸� �Ҵ� �� �ʱ�ȭ
	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));//0���� ����
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	//��Ŷ ����
	ListFile.Data.opCode = htonl(OP_CF_LISTFILE);
	ListFile.Data.dataLen = htonl(sizeof(cf_ListFile) - 8);

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

	printf_s("\n\t******************************");
	printf_s("\n\t\t���� ���丮 : %s", ListFileResp.Data.currentDir);
	if (!ListFileResp.Data.fileCount) {//������ ���� ���
		printf_s("\n\t �� ���丮 �Դϴ�.");
	}
	else {//������� ������
		printf_s("\n\t���� \t���ϸ� \t���� Ÿ��");
		for (int i = 0; i < ListFileResp.Data.fileCount; i++) {//���ϸ�� ���
			printf_s("\n\t%d. %s ", i + 1, ListFileResp.Data.fileName[i]);
			if (ListFileResp.Data.fileType[i] == 0)//���� Ÿ�� ���
				printf_s("����");
			else if (ListFileResp.Data.fileType[i] == 1)
				printf_s("����");
		}
	}
	printf_s("\n\t\t������ %d / %d", ListFileResp.Data.currentPage, ListFileResp.Data.totalPage);//������ ���
	printf_s("\n\t******************************\n");
}

/**
	@fn void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag)
	@brief DogeCloud ���� ��/�ٿ�ε�
	@author �۸۾ƾ߿��غ�
	@param hFileSrvSock ���ϼ��� ����� ����
	@param jobType 0= ���ε�, 1=�ٿ�ε�
	@param *errorFlag ���� �÷���
*/
void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag) {

}