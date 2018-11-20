#include "DogeCloud.h"

void downloadInfoFile(SOCKET hSocket) {
	cs_DownloadInfoReq DownloadInfoReq;
	sc_DownloadInfoResp DownloadInfoResp;
	memset(&DownloadInfoReq, 0, sizeof(cs_DownloadInfoReq));
	memset(&DownloadInfoResp, 0, sizeof(sc_DownloadInfoResp));

	DownloadInfoReq.Data.opCode = htonl(OP_CS_DOWNLOADUSERINFOREQ);
	DownloadInfoReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	sendData(hSocket, DownloadInfoReq.buf, sizeof(cs_DownloadInfoReq), 0);
	//다운로드 모드 시작
	FILE *downFile;
	unsigned int toRead;

	if ((downFile = fopen("myinfoClient.db", "wb+")) == NULL) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 쓰기용으로 열 수 없습니다");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	unsigned long fileSize;
	recvRaw(hSocket, &fileSize, 4, 0);
	fileSize = ntohl(fileSize);
	unsigned long left = fileSize;

	printf("\n 유저 정보 다운로드 시작...\n");

	unsigned char dataBuffer[4096]; //4KiB

	while (1) {
		if (left < 4096U)
			toRead = left;
		else
			toRead = 4096U;

		if (!recvRaw(hSocket, dataBuffer, toRead, 0)) {
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "전송 실패");
			printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
			system("pause");
			exit(1);
		}
		fwrite(dataBuffer, toRead, 1, downFile);
		left -= toRead;
		updateProgress(fileSize - left, fileSize);
		if (!left) break;//완료시 탈출
	}


	puts("\n수신 완료");
	//다운로드 모드 종료
	recvData(hSocket, DownloadInfoResp.buf, sizeof(sc_DownloadInfoResp), 0);
	DownloadInfoResp.Data.opCode = ntohl(DownloadInfoResp.Data.opCode);
	DownloadInfoResp.Data.dataLen = ntohl(DownloadInfoResp.Data.dataLen);

	unsigned char fileHash[32];
	getFileHash(downFile, fileHash);

	if (!memcmp(fileHash, DownloadInfoResp.Data.hash, 32)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "데이터 불일치");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	fclose(downFile);
	return;
}

void manageContacts(SOCKET hSocket) {
	return;
}

void manageMemo(SOCKET hSocket) {
	return;
}

void manageFile(SOCKET hSocket) {
	cs_FileSrvConReq FileSrvConReq;
	sc_FileSrvConResp FileSrvConResp;
	memset(&FileSrvConReq, 0, sizeof(cs_FileSrvConReq));
	memset(&FileSrvConResp, 0, sizeof(sc_FileSrvConResp));

	FileSrvConReq.Data.opCode = htonl(OP_CS_FILESRVCONNREQ);
	FileSrvConReq.Data.dataLen = htonl(sizeof(cs_FileSrvConReq) - 8);

	sendData(hSocket, FileSrvConReq.buf, sizeof(cs_FileSrvConReq), 0);
	recvData(hSocket, FileSrvConResp.buf, sizeof(sc_FileSrvConResp), 0);

	//convert number to host specific
	FileSrvConResp.Data.opCode = ntohl(FileSrvConResp.Data.opCode);
	FileSrvConResp.Data.dataLen = ntohl(FileSrvConResp.Data.dataLen);
	
	if (!FileSrvConResp.Data.statusCode) {
		puts("파일서버 정보를 가져올 수 없습니다!");
		puts("파일 서버에 문제가 있거나 잠시 처리가 지연되는 중일 수 있습니다. 나중에 다시 시도해주세요.");
		system("pause");
		return;
	}

	openFileServer(FileSrvConResp.Data.fileSrvAddr, FileSrvConResp.Data.fileSrvPort, FileSrvConResp.Data.authKey);
	return;
}