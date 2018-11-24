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
	@author 멍멍아야옹해봐
	@brief DogeCloud 파일서버 연결용 함수 모음
*/

#include "DogeCloud.h"

/**
	@fn void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey)
	@brief DogeCloud 파일서버 연결 및 사용자 메뉴 출력
	@author 멍멍아야옹해봐
	@param *fileServerAddr 파일 서버 주소
	@param fileServerPort 파일 서버 포트
	@param *authKey 파일 서버 인증키
*/
void openFileServer(char *fileServerAddr, unsigned long fileServerPort, unsigned char *authKey) {

	/** @brief 파일 서버 주소 저장하는 구조체 */
	SOCKADDR_IN fileSrvConAddr;

	/** @brief 파일서버 소켓 */
	SOCKET hFileSrvSock;
	memset(&fileSrvConAddr, 0, sizeof(fileSrvConAddr));

	//패킷 저장할 메모리 할당 및 0으로 초기화
	cf_LoginFile LoginFile;
	fc_LoginFileResp LoginFileResp;
	memset(&LoginFile, 0, sizeof(cf_LoginFile));//0으로 초기화
	memset(&LoginFileResp, 0, sizeof(fc_LoginFileResp));

	//파일서버 주소 구조체 설정
	fileSrvConAddr.sin_family = AF_INET;
	fileSrvConAddr.sin_port = htons(fileServerPort);
	inet_pton(AF_INET, fileServerAddr, &fileSrvConAddr.sin_addr.s_addr);

	//소켓 생성
	hFileSrvSock = socket(PF_INET, SOCK_STREAM, 0);
	if (hFileSrvSock == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, errorLevel, "Invalid File Server Socket");
		system("pause");
		return;
	}

	//파일서버 연결
	int err = (connect(hFileSrvSock, (SOCKADDR*)&fileSrvConAddr, sizeof(fileSrvConAddr)) == SOCKET_ERROR);
	if (err) //에러가 있다면
	{
		printDebugMsg(DC_ERROR, errorLevel, "파일서버 연결 실패: %d", WSAGetLastError());
		system("pause");
		return;
	}

	printDebugMsg(DC_INFO, errorLevel, "성공적으로 파일서버에 연결되었습니다.");
	Sleep(500);

	//파일서버 인증 시작
	printDebugMsg(DC_INFO, errorLevel, "파일서버 인증 시작.");

	//파일서버 로그인 패킷 설정
	memcpy(LoginFile.Data.UserFileServerAuthKey, authKey, 32);//인증키 패킷으로 복사
	LoginFile.Data.opCode = htonl(OP_CF_LOGINFILE);
	LoginFile.Data.dataLen = htonl(sizeof(cf_LoginFile) - 8);

	//파일서버에 로그인 패킷 전송
	sendData(hFileSrvSock, LoginFile.buf, sizeof(cf_LoginFile), 0);

	//로그인 응답 가져옴
	recvData(hFileSrvSock, LoginFileResp.buf, sizeof(fc_LoginFileResp), 0);

	//패킷에 있는 숫자를 호스트에 적합한 인디안으로 변경
	LoginFileResp.Data.opCode = ntohl(LoginFileResp.Data.opCode);
	LoginFileResp.Data.dataLen = ntohl(LoginFileResp.Data.dataLen);

	if (LoginFileResp.Data.statusCode == 0) {//인증이 실패한 경우 부정 접속 또는 인증 실패
		printDebugMsg(DC_ERROR, errorLevel, "파일서버 부정 접속 감지");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
		return;
	}

	//인증 성공 메시지 출력
	printDebugMsg(DC_INFO, errorLevel, "파일서버 인증 성공.");
	Sleep(500);

	//파일서버 메뉴 출력
	doFileManage(hFileSrvSock);

	//파일서버와의 작업이 종료되었을 경우
	if (closesocket(hFileSrvSock)) {//소켓 종료
		printDebugMsg(DC_ERROR, errorLevel, "파일서버 소켓 종료 실패: %d", WSAGetLastError());
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
		return;
	}

	return;
}

/**
	@fn void doFileManage(SOCKET hFileSrvSock)
	@brief DogeCloud 파일서버 메뉴 출력
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
*/
void doFileManage(SOCKET hFileSrvSock) {

	/** @brief 파일 서버 주소 저장하는 구조체 */

	/** @brief 현재 페이지 저장 */
	unsigned char page = 1; //처음 페이지는 1페이지

	while (1) {//메뉴 출력은 종료할 때 까지 무한반복

		/** @brief 에러 감지용 */
		int errorFlag = 0;

		int fileCount = 0;
		unsigned char maxPage = 0;

		system("cls");

		showFileList(hFileSrvSock, &fileCount, &page, &maxPage, &errorFlag);//파일 리스트 출력

		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 업로드");
		printf_s("\n\t2. 다운로드");
		printf_s("\n\t3. 파일 삭제");
		printf_s("\n\t4. 이전 페이지");
		printf_s("\n\t5. 다음 페이지");
		printf_s("\n\t6. 나가기");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");

		//메뉴 선택
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://업로드
			doFileJob(hFileSrvSock, 0, fileCount, &errorFlag);
			break;
		case 2://다운로드
			doFileJob(hFileSrvSock, 1, fileCount, &errorFlag);
			break;
		case 3://파일 삭제
			deleteFile(hFileSrvSock, fileCount, &errorFlag);
			break;
		case 4://이전 페이지
			if (page > 1) page--;
			break;
		case 5://다음 페이지
			if (maxPage > page) page++;
			break;
		case 6://나가기
			return;//이 함수 종료시 계속되는 코드에서 소켓 종료됨
			break;
		default: //유효하지 않은 입력
			printDebugMsg(DC_WARN, errorLevel, "올바르지 않은 입력입니다.");
			Sleep(1000);
			break;
		}

		if (errorFlag) {//에러가 감지되었을 경우
			printDebugMsg(DC_ERROR, errorLevel, "파일서버 통신 오류");
			printDebugMsg(DC_ERROR, errorLevel, "파일서버 연결을 종료합니다.");
			system("pause");
			return;//파일서버 연결 종료
		}
	}
}

/**
	@fn void showFileList(SOCKET hFileSrvSock, int *fileCount, unsigned char *page, unsigned char *maxPage, int *errorFlag)
	@brief DogeCloud 파일 목록 보여주기
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param *page 현재 페이지 변수
	@param *fileCount 파일 개수 담을 변수
	@param *maxPage 전체 페이지 수 담을 변수
	@param *errorFlag 에러 플래그
*/
void showFileList(SOCKET hFileSrvSock, int *fileCount, unsigned char *page, unsigned char *maxPage, int *errorFlag) {
	//패킷 저장할 메모리 할당 및 초기화
	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));//0으로 설정
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	//패킷 설정
	ListFile.Data.opCode = htonl(OP_CF_LISTFILE);
	ListFile.Data.dataLen = htonl(sizeof(cf_ListFile) - 8);
	ListFile.Data.page = *page;

	*errorFlag = 1;//일단 errorFlag = 1로 설정
	//패킷 전송
	sendData(hFileSrvSock, ListFile.buf, sizeof(cf_ListFile), 0);
	//응답 가져옴
	recvData(hFileSrvSock, ListFileResp.buf, sizeof(fc_ListFileResp), 0);
	*errorFlag = 0;//위 함수들이 정상적으로 종료되면 계속 함수가 진행되므로 다시 0으로 설정

	//받아온 숫자를 호스트 특정 인디안으로 변경
	ListFileResp.Data.opCode = ntohl(ListFileResp.Data.opCode);
	ListFileResp.Data.dataLen = ntohl(ListFileResp.Data.dataLen);

	if (!ListFileResp.Data.statusCode) {//실패시
		*errorFlag = 1;//파일 목록을 불러오지 못했으므로
		printDebugMsg(DC_ERROR, errorLevel, "파일서버에서 파일 목록을 가져오지 못했습니다.");
		return;
	}

	puts("\n******************************************************************************************");
	if (!ListFileResp.Data.fileCount) {//파일이 없을 경우
		printf_s("\n\t 파일이 없습니다.");
	}
	else {//비어있지 않으면
		printf_s("\n%4s %30s %30s", "순번", "파일명", "파일 용량");
		for (int i = 0; i < ListFileResp.Data.fileCount; i++) {//파일목록 출력
			char fileSize[32] = { 0, };
			minimizeFileSize(ListFileResp.Data.fileSize[i], fileSize);
			printf_s("\n%4d %30s %30s", i + 1, ListFileResp.Data.fileName[i], fileSize);
		}
	}
	printf_s("\n\t\t페이지 %d / %d", ListFileResp.Data.currentPage, ListFileResp.Data.totalPage);//페이지 출력
	puts("\n******************************************************************************************");

	*fileCount = ListFileResp.Data.fileCount;
	*maxPage = ListFileResp.Data.totalPage;
	*page = ListFileResp.Data.currentPage;

	return;
}

/**
	@fn void doFileJob(SOCKET hFileSrvSock, int jobType, int fileCount, int *errorFlag)
	@brief DogeCloud 파일 업/다운로드
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param jobType 0= 업로드, 1= 다운로드
	@param fileCount 현재 파일 개수
	@param *errorFlag 에러 플래그
*/
void doFileJob(SOCKET hFileSrvSock, int jobType, int fileCount, int *errorFlag) {
	//패킷 선언 및 초기화
	cf_FileJobReq FileJobReq;
	cffc_FileInfo FileInfo;
	fc_FileJobResult FileJobResult;
	memset(&FileJobReq, 0, sizeof(cf_FileJobReq));
	memset(&FileInfo, 0, sizeof(cffc_FileInfo));
	memset(&FileJobResult, 0, sizeof(fc_FileJobResult));

	/** @brief 임시로 생성할 파일을 위한 구조체 포인터 */
	FILE *filePtr;

	/** @brief 파일 이름 */
	char fileName[255] = { 0, };

	/** @brief 원본 파일 해쉬 */
	char orgFileHash[32] = { 0, };

	/** @brief 암호화 파일 해쉬 */
	char encFileHash[32] = { 0, };

	/** @brief 파일 사이즈 */
	unsigned long fileSize = 0;

	/** @brief 파일 비밀번호 */
	char password[100] = { 0, };

	/** @brief 암호화를 위해 해쉬로 변환된 비밀번호 */
	char pwdHash[32] = { 0, };

	/** @brief 임시로 생성할 숫자 */
	unsigned char tmpRandomNum[16] = { 0, };

	/** @brief 임시로 생성된 숫자의 해쉬 저장용 */
	unsigned char tmpHash[32] = { 0, };

	/** @brief 암복호화를 위해 임시로 생성할 파일 이름 */
	char leaFileName[65] = { 0, };

	system("cls");

	if (jobType == 0) {//업로드
		puts("업로드 할 파일을 upload 폴더에 넣으신 후 파일 이름을 입력하세요.");
		printf_s("파일 이름: ");
		scanf_s("%s", fileName, 255);

		if (strlen(fileName) > 220) {
			puts("파일 이름이 너무 깁니다. 220자 미만으로 줄여 주세요.");
			system("pause");
			return;
		}

		char tmpFileName[255];
		sprintf_s(tmpFileName, 255, "./upload/%s", fileName);

		if (fopen_s(&filePtr, tmpFileName, "rb")) {
			puts("존재하지 않는 파일입니다.");
			system("pause");
			return;
		}

		puts("파일의 암호화를 위한 비밀번호를 입력해주세요. 한 번 잊어버리면 복구 불가합니다!!");
		printf_s("비밀번호: ");
		scanf_s("%99s", password, 100);

		//비밀번호 해싱
		SHA256_Text(password, pwdHash);

		//파일 해쉬 구하기
		puts("\n해쉬 계산중입니다... 잠시만 기다려 주세요...");
		getFileHashProgress(filePtr, orgFileHash);
		puts("");
		
		
		//원본 파일 사이즈 구하기
		fseek(filePtr, 0, SEEK_END);
		fileSize = ftell(filePtr);
		rewind(filePtr);

		//임시로 사용할 파일이름 생성
		GenerateCSPRNG(tmpRandomNum, 15);
		SHA256_Text(tmpRandomNum, tmpHash);
		for (int i = 0; i < 32; i++)
			sprintf_s(leaFileName + (2 * i), 3, "%02x", tmpHash[i]);

		//암호화를 위해 임시파일 생성
		FILE *encTmpFile;
		if (fopen_s(&encTmpFile, leaFileName, "wb+")) {
			printDebugMsg(DC_ERROR, errorLevel, "파일을 쓰기용으로 열 수 없습니다");
			printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
			system("pause");
			exit(1);
		}

		//IV 생성
		unsigned char encIV[16] = { 0, };
		GenerateCSPRNG(encIV, 16);

		//암호화 하기
		puts("암호화 처리중입니다... 잠시만 기다려 주세요...");
		encryptFileLEAProgress(filePtr, encTmpFile, pwdHash, encIV);

		//암호화된 파일의 해쉬 구하기
		puts("\n해쉬 계산중입니다... 잠시만 기다려 주세요...");
		getFileHashProgress(encTmpFile, encFileHash);
		puts("");

		//요청 패킷 설정
		FileJobReq.Data.opCode = htonl(OP_CF_FILEJOBREQ);
		FileJobReq.Data.dataLen = htonl(sizeof(cf_FileJobReq) - 8);
		FileJobReq.Data.jobType = 0;

		//패킷 전송
		sendData(hFileSrvSock, FileJobReq.buf, sizeof(cf_FileJobReq), 0);

		//업로드 모드 진입
		fseek(encTmpFile, 0, SEEK_END);
		/** @brief 업로드할 파일 사이즈 */
		unsigned long uploadFileSize = ftell(encTmpFile);
		/** @brief 업로드할 남은 용량 */
		unsigned long left = uploadFileSize;
		/** @brief 읽어올 바이트 수 */
		unsigned int toRead;
		rewind(encTmpFile);

		//파일 사이즈 전송
		uploadFileSize = htonl(fileSize);
		sendData(hFileSrvSock, (char*)&uploadFileSize, 4, 0);
		uploadFileSize = ntohl(uploadFileSize);

		/**
		@var unsigned char dataBuffer[4096]
		업로드 버퍼 4KiB
		*/
		unsigned char dataBuffer[4096];

		puts("파일 전송 시작");
		while (1) {//업로드 끝날때 까지 반복
			if (left < 4096U)//4KB보다 작은만큼 남으면
				toRead = left;//남은 만큼 보냄
			else//아니면
				toRead = 4096U;//4KiB만큼 보냄

							   //파일 읽어서 버퍼에 저장
			fread(dataBuffer, toRead, 1, encTmpFile);

			//읽어온 버퍼를 서버로 전송
			if (!sendRaw(hFileSrvSock, dataBuffer, toRead, 0)) {
				printDebugMsg(DC_ERROR, errorLevel, "전송 실패");
				printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
				system("pause");
				exit(1);
			}

			left -= toRead;//보낸만큼 뺀다
			updateProgress(uploadFileSize - left, uploadFileSize);//프로그레스 바 업데이트(생성)
			if (!left) break;//완료시 탈출
		}

		puts("\n파일 전송 완료");

		//업로드 모드 끝

		//파일 정보 패킷 설정
		FileInfo.Data.opCode = htonl(OP_CFFC_FILEINFO);
		FileInfo.Data.dataLen = htonl(sizeof(cffc_FileInfo) - 8);
		FileInfo.Data.fileSize = htonl(fileSize);
		memcpy(FileInfo.Data.fileName, fileName, 255);
		memcpy(FileInfo.Data.orgFileHash, orgFileHash, 32);
		memcpy(FileInfo.Data.encFileHash, encFileHash, 32);
		memcpy(FileInfo.Data.IV, encIV, 16);

		//파일 정보 전송
		sendData(hFileSrvSock, FileInfo.buf, sizeof(cffc_FileInfo), 0);

		//응답 패킷 받음
		recvData(hFileSrvSock, FileJobResult.buf, sizeof(fc_FileJobResult), 0);

		//호스트 특정 엔디안으로 변경
		FileJobResult.Data.opCode = ntohl(FileJobResult.Data.opCode);
		FileJobResult.Data.dataLen = ntohl(FileJobResult.Data.dataLen);

		//임시파일 삭제
		fclose(encTmpFile);
		fclose(filePtr);
		remove(leaFileName);

		//성공유무 판단
		if (FileJobResult.Data.statusCode != 1) {
			puts("파일 전송이 실패하였습니다.");
			system("pause");
		}
		else {
			puts("파일 전송이 성공하였습니다.");
			system("pause");
		}
		return;
	
	}
	else if (jobType == 1) {//다운로드

	}
	else {//잘못된 입력은 진행하지 않음
		return;
	}
}

/**
	@fn void deleteFile(SOCKET hFileSrvSock, int fileCount, int *errorFlag)
	@brief DogeCloud 파일 삭제
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param fileCount 현재 파일 개수
	@param *errorFlag 에러 플래그
*/
void deleteFile(SOCKET hFileSrvSock, int fileCount, int *errorFlag) {
	//패킷 선언 및 초기화
	cf_DeleteFileReq DeleteFileReq;
	fc_DeleteFileResp DeleteFileResp;
	memset(&DeleteFileReq, 0, sizeof(DeleteFileReq));
	memset(&DeleteFileResp, 0, sizeof(DeleteFileResp));

	//유저에게 입력 받음
	unsigned long fileID;
	printf_s("몇 번 파일을 삭제하시겠습니까? (취소: 0): ");
	scanf_s("%u", &fileID);
	clearStdinBuffer();

	if (fileID == 0) return;
	if (fileID > fileCount || fileID < 0) {
		puts("유효하지 않은 입력입니다.");
		system("pause");
		return;
	}

	unsigned char userInput;
	do {
		printf_s("정말 %d번 파일을 삭제하시겠습니까? (Y/N): ", fileID);
		scanf_s("%c", &userInput, 1);
		clearStdinBuffer();
	} while (userInput != 'Y' && userInput != 'y' && userInput != 'N' && userInput != 'n');
	if (userInput == 'N' || userInput == 'n') return;
	//유저 입력 끝

	//패킷 설정
	DeleteFileReq.Data.opCode = htonl(OP_CF_DELETEFILEREQ);
	DeleteFileReq.Data.dataLen = htonl(sizeof(cf_DeleteFileReq));
	DeleteFileReq.Data.fileID = htonl(fileID);

	//패킷 전송
	sendData(hFileSrvSock, DeleteFileReq.buf, sizeof(cf_DeleteFileReq), 0);

	//응답 받기
	recvData(hFileSrvSock, DeleteFileResp.buf, sizeof(fc_DeleteFileResp), 0);

	//호스트 특정 인디안으로 변환
	DeleteFileResp.Data.opCode = ntohl(DeleteFileResp.Data.opCode);
	DeleteFileResp.Data.dataLen = ntohl(DeleteFileResp.Data.dataLen);

	if (DeleteFileResp.Data.statusCode != 1) {
		puts("삭제 실패!");
	}
	else {
		puts("삭제 성공!");
	}

	system("pause");
	return;
}