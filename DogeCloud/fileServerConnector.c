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
	int currentPage = 1; //처음 페이지는 1페이지
	while (1) {//메뉴 출력은 종료할 때 까지 무한반복

		/** @brief 에러 감지용 */
		int errorFlag = 0;

		system("cls");
		showFileList(hFileSrvSock, &errorFlag);//파일 리스트 출력

		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 업로드");
		printf_s("\n\t2. 다운로드");
		printf_s("\n\t3. 파일 삭제");
		printf_s("\n\t4. 폴더 이동");
		printf_s("\n\t5. 이전 페이지");
		printf_s("\n\t6. 다음 페이지");
		printf_s("\n\t7. 나가기");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");

		//메뉴 선택
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		switch (select) {
		case 1://업로드
			doFileJob(hFileSrvSock, 0, &errorFlag);
			break;
		case 2://다운로드
			doFileJob(hFileSrvSock, 1, &errorFlag);
			break;
		case 3://파일 삭제
			//deleteFile(hFileSrvSock);
			break;
		case 4://폴더 이동
			moveDir(hFileSrvSock, &errorFlag);
			break;
		case 5://이전 페이지로 이동
			moveFileListPage(hFileSrvSock, 0, &errorFlag);
			break;
		case 6://다음 페이지로 이동
			moveFileListPage(hFileSrvSock, 1, &errorFlag);
			break;
		case 7://나가기
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
	@fn void moveDir(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud 파일서버 디렉토리 이동
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param *errorFlag 에러 플래그
*/
void moveDir(SOCKET hFileSrvSock, int *errorFlag) {
	//패킷 저장할 메모리 할당 및 초기화
	cf_MoveDir MoveDir;
	fc_MoveDirResp MoveDirResp;
	memset(&MoveDir, 0, sizeof(cf_MoveDir));//0으로 초기화
	memset(&MoveDirResp, 0, sizeof(fc_MoveDirResp));

	//유저에게 이동할 디렉토리 받아옴
	char directory[255];
	printf_s("이동할 디렉토리 입력(최대 254자) : ");
	scanf_s("%s", directory, 255);

	//디렉토리 이동 패킷 설정
	MoveDir.Data.opCode = htonl(OP_CF_MOVEDIR);
	MoveDir.Data.dataLen = htonl(sizeof(cf_MoveDir) - 8);
	strcpy_s(MoveDir.Data.moveDir, 255, directory);

	*errorFlag = 1;//일단 errorFlag = 1로 설정
	//디렉토리 이동 패킷 전송
	sendData(hFileSrvSock, MoveDir.buf, sizeof(cf_MoveDir), 0);
	//응답 받아옴
	recvData(hFileSrvSock, MoveDirResp.buf, sizeof(fc_MoveDirResp), 0);
	*errorFlag = 0;//위 함수들이 정상적으로 종료되면 계속 함수가 진행되므로 다시 0으로 설정

	//받아온 숫자를 호스트 특정 인디안으로 변경
	MoveDirResp.Data.opCode = ntohl(MoveDirResp.Data.opCode);
	MoveDirResp.Data.dataLen = ntohl(MoveDirResp.Data.dataLen);

	if (!MoveDirResp.Data.statusCode) {//디렉토리 변경 실패시
		printDebugMsg(DC_WARN, errorLevel, "존재하지 않는 디렉토리이거나 요청이 실패했습니다.");
	}

	return;
}

/**
	@fn void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag)
	@brief DogeCloud 파일 목록 페이지 이동
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param type 0= 이전 페이지, 1=다음 페이지
	@param *errorFlag 에러 플래그
*/
void moveFileListPage(SOCKET hFileSrvSock, char type, int *errorFlag) {
	//패킷 저장할 메모리 할당 및 초기화
	cf_ListPageMove ListPageMove;
	fc_ListPageMoveResp ListPageMoveResp;
	memset(&ListPageMove, 0, sizeof(cf_ListPageMove));//0으로 초기화
	memset(&ListPageMoveResp, 0, sizeof(fc_ListPageMoveResp));

	//페이지 이동용 패킷 설정
	ListPageMove.Data.opCode = htonl(OP_CF_LISTPAGEMOVE);
	ListPageMove.Data.dataLen = htonl(sizeof(cf_ListPageMove) - 8);
	ListPageMove.Data.moveTo = type;

	*errorFlag = 1;//일단 errorFlag = 1로 설정
	//페이지 이동 패킷 전송
	sendData(hFileSrvSock, ListPageMove.buf, sizeof(cf_ListPageMove), 0);
	//응답 가져옴
	recvData(hFileSrvSock, ListPageMoveResp.buf, sizeof(fc_ListPageMoveResp), 0);
	*errorFlag = 0;//위 함수들이 정상적으로 종료되면 계속 함수가 진행되므로 다시 0으로 설정

	//받아온 숫자를 호스트 특정 인디안으로 변경
	ListPageMove.Data.opCode = ntohl(ListPageMove.Data.opCode);
	ListPageMove.Data.dataLen = ntohl(ListPageMove.Data.dataLen);

	if (!ListPageMoveResp.Data.statusCode) {//실패시
		*errorFlag = 1;
		printDebugMsg(DC_ERROR, errorLevel, "요청이 실패했습니다.");
	}

	return;
}

/**
	@fn void showFileList(SOCKET hFileSrvSock, int *errorFlag)
	@brief DogeCloud 파일 목록 보여주기
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param *errorFlag 에러 플래그
*/
void showFileList(SOCKET hFileSrvSock, int *errorFlag) {
	//패킷 저장할 메모리 할당 및 초기화
	cf_ListFile ListFile;
	fc_ListFileResp ListFileResp;
	memset(&ListFile, 0, sizeof(cf_ListFile));//0으로 설정
	memset(&ListFileResp, 0, sizeof(fc_ListFileResp));

	//패킷 설정
	ListFile.Data.opCode = htonl(OP_CF_LISTFILE);
	ListFile.Data.dataLen = htonl(sizeof(cf_ListFile) - 8);

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

	printf_s("\n\t******************************");
	printf_s("\n\t\t현재 디렉토리 : %s", ListFileResp.Data.currentDir);
	if (!ListFileResp.Data.fileCount) {//파일이 없을 경우
		printf_s("\n\t 빈 디렉토리 입니다.");
	}
	else {//비어있지 않으면
		printf_s("\n\t순번 \t파일명 \t파일 타입");
		for (int i = 0; i < ListFileResp.Data.fileCount; i++) {//파일목록 출력
			printf_s("\n\t%d. %s ", i + 1, ListFileResp.Data.fileName[i]);
			if (ListFileResp.Data.fileType[i] == 0)//파일 타입 출력
				printf_s("파일");
			else if (ListFileResp.Data.fileType[i] == 1)
				printf_s("폴더");
		}
	}
	printf_s("\n\t\t페이지 %d / %d", ListFileResp.Data.currentPage, ListFileResp.Data.totalPage);//페이지 출력
	printf_s("\n\t******************************\n");
}

/**
	@fn void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag)
	@brief DogeCloud 파일 업/다운로드
	@author 멍멍아야옹해봐
	@param hFileSrvSock 파일서버 연결된 소켓
	@param jobType 0= 업로드, 1=다운로드
	@param *errorFlag 에러 플래그
*/
void doFileJob(SOCKET hFileSrvSock, int jobType, int *errorFlag) {

}