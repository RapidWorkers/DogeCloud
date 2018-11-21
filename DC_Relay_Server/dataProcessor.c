/*
삭제 예정
*/

#include "RelayServer.h"

void procFileUpDemo(SOCKET hClientSock) {
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "File Upload Started...");
	FILE *downFile;

	if ((downFile = fopen("Server_downloaddFromClient", "wb+")) == NULL) {
		printf("ERROR");
		exit(1);
	}

	unsigned long fileSize;
	recv(hClientSock, (char*)&fileSize, 4, 0);
	fileSize = ntohl(fileSize);

	printf("File Size is %d byte", fileSize);

	unsigned char dataBuffer[2048]; //2KiB
	int percentage = 0;

	int cnt = 0;
	for (int i = 0; i < fileSize / 2048; i++, cnt++) {
		int remainData = 2048;
		
		while (remainData > 0) {
			int strLen = recv(hClientSock, dataBuffer, remainData, 0);
			remainData -= strLen;
		}
		
		fwrite(dataBuffer, 2048, 1, downFile);
		percentage = ((double)(2048 * i) / fileSize)*100;
		if ((i * 2048) % (2 * 1024 * 1024) == 0) printf("Recv: %d %%\n", percentage);
	}

	cnt = cnt;

	fclose(downFile);
	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "File Upload Done...");
}

void procFileDownDemo(SOCKET hClientSock) {
	FILE *randFile;

	if (fopen_s(&randFile, "Server_randomGenUpFile", "wb+")) {
		printf("ERROR");
		exit(1);
	}

	printf("Creating Files.. Please Wait... (Server Still able to Accept new client!)\n");
	unsigned long fileSize = 200 * 1024 * 1024; //200MiB
	int temp_Data;
	GenerateCSPRNG(&temp_Data, 4);
	for (int i = 0; i < fileSize / 4; i++) {
		fwrite(&temp_Data, 4, 1, randFile);
	}
	fseek(randFile, 0, SEEK_SET);

	fileSize = htonl(fileSize);
	send(hClientSock, &fileSize, 4, 0);//send file size
	fileSize = ntohl(fileSize);

	unsigned char dataBuffer[2048]; //2KiB

	int percentage = 0;

	int cnt = 0;
	for (int i = 0; i < fileSize / 2048; i++, cnt++) {
		fread(dataBuffer, 2048, 1, randFile);
		percentage = ((double)(2048 * i) / fileSize) * 100;
		if ((i * 2048) % (2 * 1024 * 1024) == 0) printf("Sent: %d %%\n", percentage);
		send(hClientSock, dataBuffer, 2048, 0);
	}

	fclose(randFile);
	printf("Transfer Finished");
}