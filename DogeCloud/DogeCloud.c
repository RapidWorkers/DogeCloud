#include "DogeCloud.h"

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

		//printf_s("\n\t1. 파일업로드 데모");
		//printf_s("\n\t2. 파일다운로드 데모");
		//printf_s("\n\t3. SHA 테스트");
		//printf_s("\n\t4. LEA 암복호화 테스트");
		//printf_s("\n\t5. 로그아웃");
		//printf_s("\n\t6. 종료");
		printf_s("\n\t******************************");
		printf_s("\n\t메뉴 선택 : ");
		break;
	default://유효하지 않은 로그인 상태일 때
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "유효하지 않은 LoginFlag 상태입니다. 메모리 변조 의심!!\n");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다..");
		system("pause");
		exit(1);
		break;
	}
}

//session related var
int loginFlag = 0;
char sessionKey[32] = { 0, };
char currentUsername[100] = { 0, };

int initProgram(WSADATA *wsaData, SOCKET *hRelayServSocket, SOCKADDR_IN *RelayServAddr) {
	printProgramInfo();

	if (WSAStartup(MAKEWORD(2, 2), wsaData) != 0) { //소켓 라이브러리 초기화
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "WSAStartup 실패!\n");
		return 0;
	}

	*hRelayServSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (*hRelayServSocket == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "유효하지 않은 소켓입니다.\n");
		return 0;
	}

	readRelayServerPath(RelayServAddr);

	int err = (connect(*hRelayServSocket, (SOCKADDR*)RelayServAddr, sizeof(*RelayServAddr)) == SOCKET_ERROR);
	if (err) //생성된 소켓으로 서버에 연결
	{
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "중계서버와의 연결이 실패했습니다: %d", WSAGetLastError());
		return 0;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "성공적으로 중계서버에 연결되었습니다.\n");
	Sleep(500);
	return 1;
}

int main() {

	WSADATA wsaData;
	SOCKET hRelayServSocket, hFileServSocket;
	SOCKADDR_IN RelayServAddr, FileServAddr;

	if (!initProgram(&wsaData, &hRelayServSocket, &RelayServAddr)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램 초기화 실패");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	printMenu();

	while (1) {

		printMenu();
		int select;
		scanf_s("%d", &select);
		clearStdinBuffer();

		system("cls");
		printProgramInfo();

		if (loginFlag == 0) {
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
				printDebugMsg(DC_WARN, DC_ERRORLEVEL, "올바르지 않은 입력입니다.");
				Sleep(1000);
				break;
			}
		}
		else if (loginFlag == 1) {
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
				userLogout(hRelayServSocket);
				closesocket(hRelayServSocket); //소켓 해제
				printf("Closed!\n");
				WSACleanup();
				system("pause");
				exit(0);
				break;
			default:
				printDebugMsg(DC_WARN, DC_ERRORLEVEL, "올바르지 않은 입력입니다.");
				Sleep(1000);
				break;
			}
		}
	}

	closesocket(hRelayServSocket); //소켓 라이브러리 해제
	printf("Closed!\n");
	WSACleanup();
	system("pause");
}