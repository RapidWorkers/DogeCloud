#include "DogeCloud.h"

void printMenu() {
	system("cls");
	printProgramInfo();

	switch (loginFlag) {//Login ���¿� ���� �޴� ǥ��
	case 0://������ �α��� ���� ���� ����
		printf_s("\n\t*********  ��   ��   *********");
		printf_s("\n\t1. �α���");
		printf_s("\n\t2. ȸ�� ����");
		printf_s("\n\t3. ����");
		printf_s("\n\t4. ���α׷� ���� �� ���̼���");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");
		break;
	case 1://������ �α��� ���� ���
		printf_s("\n���� �α��� �����: %s", currentUsername);
		printf_s("\n\t*********  ��   ��   *********");

		printf_s("\n\t1. �޸� ����");
		printf_s("\n\t2. ����ó ����");
		printf_s("\n\t3. ���� ����");
		printf_s("\n\t4. �α׾ƿ�");
		printf_s("\n\t5. ����");

		//printf_s("\n\t1. ���Ͼ��ε� ����");
		//printf_s("\n\t2. ���ϴٿ�ε� ����");
		//printf_s("\n\t3. SHA �׽�Ʈ");
		//printf_s("\n\t4. LEA �Ϻ�ȣȭ �׽�Ʈ");
		//printf_s("\n\t5. �α׾ƿ�");
		//printf_s("\n\t6. ����");
		printf_s("\n\t******************************");
		printf_s("\n\t�޴� ���� : ");
		break;
	default://��ȿ���� ���� �α��� ������ ��
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "��ȿ���� ���� LoginFlag �����Դϴ�. �޸� ���� �ǽ�!!\n");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�..");
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

	if (WSAStartup(MAKEWORD(2, 2), wsaData) != 0) { //���� ���̺귯�� �ʱ�ȭ
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "WSAStartup ����!\n");
		return 0;
	}

	*hRelayServSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (*hRelayServSocket == INVALID_SOCKET) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "��ȿ���� ���� �����Դϴ�.\n");
		return 0;
	}

	readRelayServerPath(RelayServAddr);

	int err = (connect(*hRelayServSocket, (SOCKADDR*)RelayServAddr, sizeof(*RelayServAddr)) == SOCKET_ERROR);
	if (err) //������ �������� ������ ����
	{
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "�߰輭������ ������ �����߽��ϴ�: %d", WSAGetLastError());
		return 0;
	}

	printDebugMsg(DC_INFO, DC_ERRORLEVEL, "���������� �߰輭���� ����Ǿ����ϴ�.\n");
	Sleep(500);
	return 1;
}

int main() {

	WSADATA wsaData;
	SOCKET hRelayServSocket, hFileServSocket;
	SOCKADDR_IN RelayServAddr, FileServAddr;

	if (!initProgram(&wsaData, &hRelayServSocket, &RelayServAddr)) {
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷� �ʱ�ȭ ����");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "���α׷��� �����մϴ�.");
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
			case 1: //�α���
				userLogin(hRelayServSocket);
				break;
			case 2: //ȸ�� ����
				userRegister(hRelayServSocket);
				break;
			case 3: //����
				closesocket(hRelayServSocket); //���� ���̺귯�� ����
				WSACleanup();
				exit(0);
				break;
			case 4: //���α׷� ���̼��� ǥ��
				printProgramInfo();
				printLicense();
				system("pause");
				break;
			default: //��ȿ���� ���� �Է�
				printDebugMsg(DC_WARN, DC_ERRORLEVEL, "�ùٸ��� ���� �Է��Դϴ�.");
				Sleep(1000);
				break;
			}
		}
		else if (loginFlag == 1) {
			switch (select) {
			case 1: //�޸� ����
				manageMemo(hRelayServSocket);
				break;
			case 2: //����ó ����
				manageContacts(hRelayServSocket);
				break;
			case 3: //���� ����
				manageFile(hRelayServSocket);
				break;
			case 4: //�α׾ƿ�
				userLogout(hRelayServSocket);
				break;
			case 5: //����
				userLogout(hRelayServSocket);
				closesocket(hRelayServSocket); //���� ����
				printf("Closed!\n");
				WSACleanup();
				system("pause");
				exit(0);
				break;
			default:
				printDebugMsg(DC_WARN, DC_ERRORLEVEL, "�ùٸ��� ���� �Է��Դϴ�.");
				Sleep(1000);
				break;
			}
		}
	}

	closesocket(hRelayServSocket); //���� ���̺귯�� ����
	printf("Closed!\n");
	WSACleanup();
	system("pause");
}