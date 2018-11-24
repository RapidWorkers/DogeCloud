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
	@file DogeCloud.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief DogeCloud Main 파일
*/

#include "DogeCloud.h"

/**
	@fn void printMenu()
	@brief DogeCloud 메인 메뉴 출력
	@author 멍멍아야옹해봐
*/
void printMenu() {
	system("cls");
	printProgramInfo();

	switch (loginFlag) {//Login 상태에 따른 메뉴 표시
	case 0://유저가 로그인 하지 않은 상태
		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 로그인");
		printf_s("\n\t2. 회원 가입");
		printf_s("\n\t3. 종료");
		printf_s("\n\t4. 프로그램 정보 및 라이센스");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");
		break;
	case 1://유저가 로그인 했을 경우
		printf_s("\n현재 로그인 사용자: %s", currentUsername);
		printf_s("\n\t*********  메   뉴   *********");
		printf_s("\n\t1. 메모 관리");
		printf_s("\n\t2. 연락처 관리");
		printf_s("\n\t3. 파일 관리");
		printf_s("\n\t4. 로그아웃");
		printf_s("\n\t5. 종료");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");
		break;
	default://유효하지 않은 로그인 상태일 때
		printDebugMsg(DC_ERROR, errorLevel, "유효하지 않은 LoginFlag 상태입니다. 메모리 변조 의심!!\n");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다..");
		system("pause");
		exit(1);
		break;
	}
}

/**
	@fn int initProgram(WSADATA *wsaData, SOCKET *hRelayServSocket, SOCKADDR_IN *RelayServAddr)
	@brief DogeCloud 중계 서버 연결
	@author 멍멍아야옹해봐
	@param *wsaData WSADATA 구조체 주소
	@param *hRelayServSocket SOCKET 구조체 주소
	@param *RelayServAddr SOCKADDR_IN 구조체 주소
	@return 0 = 실패, 1 = 성공
*/
int initProgram(WSADATA *wsaData, SOCKET *hRelayServSocket, SOCKADDR_IN *RelayServAddr) {
	printProgramInfo();

	//업로드 폴더 생성
	if (!CreateDirectory("./upload", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {//이미 폴더가 존재할 때의 경우를 제외한 모든 에러는 프로그램 정지
			printDebugMsg(DC_ERROR, errorLevel, "Create Directory Fail");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}
	};

	//다운로드 폴더 생성
	if (!CreateDirectory("./download", NULL)) {
		if (GetLastError() != ERROR_ALREADY_EXISTS) {//이미 폴더가 존재할 때의 경우를 제외한 모든 에러는 프로그램 정지
			printDebugMsg(DC_ERROR, errorLevel, "Create Directory Fail");
			printDebugMsg(DC_ERROR, errorLevel, "Exiting Program");
			system("pause");
			exit(1);
		}
	};

	if (WSAStartup(MAKEWORD(2, 2), wsaData) != 0) { //소켓 라이브러리 초기화
		printDebugMsg(DC_ERROR, errorLevel, "WSAStartup 실패!\n");
		return 0;
	}

	*hRelayServSocket = socket(PF_INET, SOCK_STREAM, 0); //소켓 초기화
	if (*hRelayServSocket == INVALID_SOCKET) {//소켓 생성 성공했는지 검사
		printDebugMsg(DC_ERROR, errorLevel, "유효하지 않은 소켓입니다.\n");
		return 0;
	}

	readRelayServerPath(RelayServAddr); //중계서버 주소 설정파일에서 읽어옴

	int err = (connect(*hRelayServSocket, (SOCKADDR*)RelayServAddr, sizeof(*RelayServAddr)) == SOCKET_ERROR);//중계서버에 연결
	if (err) //에러 검사
	{
		printDebugMsg(DC_ERROR, errorLevel, "중계서버와의 연결이 실패했습니다: %d", WSAGetLastError());
		return 0;
	}

	printDebugMsg(DC_INFO, errorLevel, "성공적으로 중계서버에 연결되었습니다.\n");
	Sleep(500);
	return 1;
}

//세션 관련 전역 변수
/**
	@var int loginFlag
	로그인 여부 저장
*/
int loginFlag = 0;

/**
	@var char currentUsername[100]
	현재 로그인된 유저 이름 저장
*/
char sessionKey[32] = { 0, };

/**
	@var char sessionKey[32]
	중계서버 인증 세션키 저장용 변수
*/
char currentUsername[100] = { 0, };

/**
	@var int errorLevel
	중계서버 인증 세션키 저장용 변수
*/
int errorLevel;

/**
	@fn int main()
	@brief DogeCloud 진입점(Main)
	@author 멍멍아야옹해봐
*/
int main() {

	/** @brief 라이브러리용 구조체 */
	WSADATA wsaData;

	/** @brief 서버 연결용 소켓 구조체 */
	SOCKET hRelayServSocket;

	/** @brief 서버 주소 저장하는 구조체 */
	SOCKADDR_IN RelayServAddr;

	//에러레벨 읽고 설정
	setErrorLevel();

	if (!initProgram(&wsaData, &hRelayServSocket, &RelayServAddr)) {//initProgram 함수로 초기화 및 연결
		printDebugMsg(DC_ERROR, errorLevel, "프로그램 초기화 실패");
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	printMenu();

	while (1) {//메뉴 선택은 종료할때까지 무한반복
		printMenu();

		int select;//유저 입력 저장용
		scanf_s("%d", &select);
		clearStdinBuffer();

		//입력 받고 화면 비우기
		system("cls");
		printProgramInfo();

		if (loginFlag == 0) {//로그인을 안했다면
			switch (select) {
			case 1: //로그인
				userLogin(hRelayServSocket);
				break;
			case 2: //회원 가입
				userRegister(hRelayServSocket);
				break;
			case 3: //종료
				closesocket(hRelayServSocket); //소켓 라이브러리 해제
				WSACleanup();
				exit(0);
				break;
			case 4: //프로그램 라이센스 표시
				printProgramInfo();
				printLicense();
				system("pause");
				break;
			default: //유효하지 않은 입력
				printDebugMsg(DC_WARN, errorLevel, "올바르지 않은 입력입니다.");
				Sleep(1000);
				break;
			}
		}
		else if (loginFlag == 1) {//로그인을 했다면
			switch (select) {
			case 1: //메모 관리
				manageMemo(hRelayServSocket);
				break;
			case 2: //연락처 관리
				manageContacts(hRelayServSocket);
				break;
			case 3: //파일 관리
				manageFile(hRelayServSocket);
				break;
			case 4: //로그아웃
				userLogout(hRelayServSocket);
				break;
			case 5: //종료
				userLogout(hRelayServSocket);//먼저 로그아웃
				closesocket(hRelayServSocket); //소켓 해제
				printf_s("서버 연결 종료!\n");
				WSACleanup();//소켓 라이브러리 해제
				system("pause");
				exit(0);//종료
				break;
			default://올바르지 않은 입력
				printDebugMsg(DC_WARN, errorLevel, "올바르지 않은 입력입니다.");
				Sleep(1000);
				break;
			}
		}
	}

	closesocket(hRelayServSocket); //소켓 연결 해제
	printf_s("서버 연결 종료!\n");
	WSACleanup();//소켓 라이브러리 해제
	system("pause");

	return 0;
}