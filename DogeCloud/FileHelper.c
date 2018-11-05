#include "DogeCloud.h"

void fileDownDemo(SOCKET hSocket) {
	system("cls");
	printProgramInfo();
	printf("�� ����� 200MB�� ������ �̷���� ������ �������� �ٿ�ε� �޽��ϴ�.\n");

	unsigned char opCode[4];
	long tmp_opCode = htonl(251);
	memcpy(opCode, &tmp_opCode, 4);
	printf("���� �����.. ��ø� ��ٷ� �ֽʽÿ�...");
	sendRaw(hSocket, opCode, 4, 0);//send opcode for demo
	FILE *downFile;

	if ((downFile = fopen("Client_downloaddFromServer", "wb+")) == NULL) {
		printf("ERROR");
		exit(1);
	}

	unsigned long fileSize;
	recv(hSocket, &fileSize, 4, 0);
	fileSize = ntohl(fileSize);

	printf("File Size is %d byte\n", fileSize);

	unsigned char dataBuffer[2048]; //2KiB
	int percentage = 0;

	int cnt = 0;
	for (int i = 0; i < fileSize / 2048; i++, cnt++) {
		if (!recvRaw(hSocket, dataBuffer, 2048, 0)) {
			printf("���� ����!!!!\n");
		}
		fwrite(dataBuffer, 2048, 1, downFile);
		percentage = ((double)(2048 * i) / fileSize) * 100;
		if ((i * 2048) % (2 * 1024 * 1024) == 0) printf("����: %d %%\n", percentage);
	}

	cnt = cnt;

	fclose(downFile);
	printf("���� �Ϸ�: ���� Ȯ�� �ٶ�");
	system("pause");
}

void fileUpDemo(SOCKET hSocket) {
	system("cls");
	printProgramInfo();
	printf("�� ����� 200MB�� ������ �����Ͽ� ������ ���ε带 �׽�Ʈ �մϴ�.\n");

	FILE *randFile;

	if ((randFile = fopen("Client_randomGenUpFile", "wb+")) == NULL) {
		printf("ERROR");
		exit(1);
	}

	printf("���� ������... ��ø� ��ٷ� �ֽʽÿ�...\n");
	unsigned long fileSize = 200 * 1024 * 1024; //200MiB
	int temp_Data = GenerateCSPRNG();
	for (int i = 0; i < fileSize / 4; i++) {
		fwrite(&temp_Data, 4, 1, randFile);
	}
	fseek(randFile, 0, SEEK_SET);
	printf("���� ������... ��ø� ��ٷ� �ֽʽÿ�...\n");
	unsigned char opCode[4];
	long tmp_opCode = htonl(250);
	memcpy(opCode, &tmp_opCode, 4);
	fileSize = htonl(fileSize);
	

	sendRaw(hSocket, opCode, 4, 0);//send opcode for demo
	sendRaw(hSocket, &fileSize, 4, 0);//send file size
	fileSize = ntohl(fileSize);

	unsigned char dataBuffer[2048]; //2KiB

	int percentage = 0;

	int cnt = 0;
	for (int i = 0; i < fileSize / 2048; i++, cnt++) {
		fread(dataBuffer, 2048, 1, randFile);
		percentage = ((double)(2048 * i) / fileSize) * 100;
		if ((i * 2048) % (2 * 1024 * 1024) == 0) printf("������: %d %%\n", percentage);
		sendRaw(hSocket, dataBuffer, 2048, 0);
	}
	cnt = cnt;
	fclose(randFile);
	printf("���� �Ϸ�: ���� Ȯ�� �ٶ�");
	system("pause");
}

void testSHA() {
	FILE *testFile = fopen("testFile.txt", "r");
	unsigned char hashed[32];
	getFileHash(testFile, hashed);

	printf("HASH : ");
	for (int i = 0; i < 32; i++)
		printf("%2X ", hashed[i]);
	system("pause");
}