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
	@file AuthHelper.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief DogeCloud 로그인 로그아웃 관련 함수 모음
*/

#include "DogeCloud.h"

/**
	@fn void userLogin(SOCKET hSocket)
	@brief DogeCloud 중계 서버 로그인
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void userLogin(SOCKET hSocket) {
	//패킷을 저장할 공간 할당
	cs_LoginStart LoginStart;
	sc_LoginStartResp LoginStartResp;
	cs_LoginAccountData LoginAccountData;
	sc_LoginDoneResp LoginDoneResp;

	//0으로 초기화
	memset(&LoginStart, 0, sizeof(LoginStart));
	memset(&LoginStartResp, 0, sizeof(LoginStartResp));
	memset(&LoginAccountData, 0, sizeof(LoginAccountData));
	memset(&LoginDoneResp, 0, sizeof(LoginDoneResp));

	//유저 아이디 및 비밀번호 저장용 배열 할당 및 초기화
	char uname[100] = { 0, }, pwd[100] = { 0, };

	//패킷 설정
	LoginStart.Data.opCode = htonl(OP_CS_LOGINSTART);
	LoginStart.Data.dataLen = htonl(sizeof(cs_LoginStart) - 8);
	LoginStart.Data.clientVersion = htonl(20181020);

	//로그인 시작 패킷 전송
	sendData(hSocket, LoginStart.buf, sizeof(cs_LoginStart), 0);

	//로그인 응답 패킷 받기
	recvData(hSocket, LoginStartResp.buf, sizeof(sc_LoginStartResp), 0);

	//전달받은 숫자를 Big Endian에서 호스트 환경 인디안으로 변경
	LoginStartResp.Data.opCode = ntohl(LoginStartResp.Data.opCode);
	LoginStartResp.Data.dataLen = ntohl(LoginStartResp.Data.dataLen);

	//사용자에게서 아이디와 비밀번호 가져옴
	puts("100자를 초과하는 입력은 자동으로 잘립니다.");
	printf("아이디 (최대 99자): ");
	scanf_s("%99s", uname, 100);
	clearStdinBuffer();
	printf("비밀번호 (최대 99자): ");
	scanf_s("%99s", pwd, 100);
	clearStdinBuffer();

	//로그인 정보 패킷 초기화
	LoginAccountData.Data.opCode = htonl(OP_CS_LOGINACCOUNTDATA);
	LoginAccountData.Data.dataLen = htonl(sizeof(cs_LoginAccountData) - 8);
	memcpy(&(LoginAccountData.Data.Username), uname, 100);
	memcpy(&(LoginAccountData.Data.Password), pwd, 100);

	//로그인 정보 설정 
	sendData(hSocket, LoginAccountData.buf, sizeof(LoginAccountData), 0)

	//로그인 완료 패킷 서버에서 가져옴
	recvData(hSocket, LoginDoneResp.buf, sizeof(LoginDoneResp), 0)

	//전달받은 숫자를 Big Endian에서 호스트 환경 인디안으로 변경
	LoginDoneResp.Data.opCode = ntohl(LoginDoneResp.Data.opCode);
	LoginDoneResp.Data.dataLen = ntohl(LoginDoneResp.Data.dataLen);

	if (LoginDoneResp.Data.statusCode == 0) puts("로그인 실패!!!");//로그인 실패시
	else if (LoginDoneResp.Data.statusCode == 1)//로그인 성공시
	{
		puts("로그인 성공!!!");
		loginFlag = 1; //로그인 플래그 설정
		memcpy(sessionKey, LoginDoneResp.Data.sessionKey, 32); //세션키를 패킷에서 전역변수로 복사
		memcpy(currentUsername, uname, 100); //아이디를 전역변수로 복사
		downloadPersonalDBFile(hSocket);//개인 DB 파일을 서버에서 다운로드
	}
	else {//그 외의 값을 전달받은 경우
		printDebugMsg(DC_ERROR, errorLevel, "중계서버에서 예상하지 못한 데이터를 전송받았습니다. statusCode: %d", LoginDoneResp.Data.statusCode);
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
	}

	system("pause");
	return;
}

/**
	@fn void userLogout(SOCKET hSocket)
	@brief DogeCloud 중계 서버 로그아웃
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void userLogout(SOCKET hSocket) {

	//패킷을 위한 메모리 할당 및 초기화
	cs_LogoutStart LogoutStart;
	sc_LogoutDone LogoutDone;
	memset(&LogoutStart, 0, sizeof(LogoutStart));//0으로 초기화
	memset(&LogoutDone, 0, sizeof(LogoutDone));

	//로그아웃 시작 패킷 설정
	LogoutStart.Data.opCode = htonl(OP_CS_LOGOUTSTART);
	LogoutStart.Data.dataLen = htonl(sizeof(cs_LogoutStart) - 8);

	//로그아웃 시작 패킷 전송
	sendData(hSocket, LogoutStart.buf, sizeof(LogoutStart), 0);

	//서버에서 로그인 완료 패킷 받아옴
	recvData(hSocket, LogoutDone.buf, sizeof(LogoutDone), 0);

	if (LogoutDone.Data.statusCode == 1) {//로그아웃 성공시
		puts("로그아웃 성공!");
		memset(sessionKey, 0, sizeof(sessionKey)); //세션키 전역변수에서 제거
		memset(currentUsername, 0, sizeof(currentUsername)); //아이디도 제거
		loginFlag = 0; //로그인 플래그 설정
	}
	else//실패 또는 그 외의 응답시
		puts("로그아웃 실패!");

	system("pause");
	return;
}

/**
	@fn void userRegister(SOCKET hSocket)
	@brief DogeCloud 회원가입
	@author 멍멍아야옹해봐
	@param hSocket 중계 서버 연결된 소켓
*/
void userRegister(SOCKET hSocket) {
	//패킷을 위한 메모리 할당 및 초기화
	cs_RegisterStart RegisterStart;
	sc_RegisterStartResp RegisterStartResp;
	cs_RegisterAccountData RegisterAccountData;
	sc_RegisterDone RegisterDone;
	memset(&RegisterStart, 0, sizeof(RegisterStart));//0으로 초기화
	memset(&RegisterStartResp, 0, sizeof(RegisterStartResp));
	memset(&RegisterAccountData, 0, sizeof(RegisterAccountData));
	memset(&RegisterDone, 0, sizeof(RegisterDone));

	//유저 입력을 위한 배열 선언 및 초기화
	char email[100] = { 0, };
	char uname[100] = { 0, };
	char pwd[100] = { 0, };

	system("cls");
	printProgramInfo();

	//회원가입 시작 패킷 설정
	RegisterStart.Data.opCode = htonl(OP_CS_REGISTERSTART);
	RegisterStart.Data.dataLen = htonl(sizeof(RegisterStart) - 8);
	RegisterStart.Data.clientVersion = htonl(20181020);

	//회원가입 시작 패킷 전송
	sendData(hSocket, RegisterStart.buf, sizeof(RegisterStart), 0);

	//회원가입 시작 응답 패킷 서버에서 받아옴
	recvData(hSocket, RegisterStartResp.buf, sizeof(RegisterStartResp), 0)
	
	//전달받은 숫자를 Big Endian에서 호스트 환경 인디안으로 변경
	RegisterStartResp.Data.opCode = ntohl(RegisterStartResp.Data.opCode);
	RegisterStartResp.Data.dataLen = ntohl(RegisterStartResp.Data.dataLen);

	if (RegisterStartResp.Data.statusCode == 0) {//만약 회원가입 불가 상태라면
		printDebugMsg(DC_ERROR, errorLevel, "현재 회원가입은 불가능합니다. 나중에 다시 시도해 주세요.");
		system("pause");
		return;
	}

	//사용자로부터 이메일, 아이디, 비밀번호 받아옴
	puts("99자를 초과하는 입력은 자동으로 잘립니다.");
	printf("이메일 (최대 99자): ");
	scanf_s("%99s", email, 100);
	clearStdinBuffer();
	printf("아이디 (최대 99자): ");
	scanf_s("%99s", uname, 100);
	clearStdinBuffer();
	printf("비밀번호 (최대 99자): ");
	scanf_s("%99s", pwd, 100);
	clearStdinBuffer();

	//회원가입 정보 패킷 설정
	RegisterAccountData.Data.opCode = htonl(OP_CS_REGISTERACCOUNTDATA);
	RegisterAccountData.Data.dataLen = htonl(sizeof(RegisterAccountData) - 8);
	memcpy_s(RegisterAccountData.Data.email, 100, email, 100);
	memcpy_s(RegisterAccountData.Data.Username, 100, uname, 100);
	memcpy_s(RegisterAccountData.Data.Password, 100, pwd, 100);

	//회원가입 정보 패킷 전송
	sendData(hSocket, RegisterAccountData.buf, sizeof(RegisterAccountData), 0);

	//회원가입 완료 패킷 가져옴
	recvData(hSocket, RegisterDone.buf, sizeof(RegisterDone), 0);

	//전달받은 숫자를 Big Endian에서 호스트 환경 인디안으로 변경
	RegisterDone.Data.opCode = ntohl(RegisterDone.Data.opCode);
	RegisterDone.Data.dataLen = ntohl(RegisterDone.Data.dataLen);

	switch (RegisterDone.Data.statusCode) {//회원가입 성공 유무에 따라 분기
	case 0://실패시
		puts("회원 가입 실패!!!");
		break;
	case 1://아이디 중복
		puts("아이디 중복!!!");
		break;
	case 2://성공
		puts("회원가입 및 로그인 성공!!!");
		loginFlag = 1; //로그인 플래그 설정
		memcpy(sessionKey, RegisterDone.Data.sessionKey, 32); //전역변수로 세션키 복사
		memcpy(currentUsername, uname, 100); //전역변수로 아이디 복사
		downloadPersonalDBFile(hSocket); //개인 DB 파일 서버에서 다운로드
		break;
	default://그 외 응답
		printDebugMsg(DC_ERROR, errorLevel, "중계서버에서 예상하지 못한 데이터를 전송받았습니다. statusCode: %d", RegisterDone.Data.statusCode);
		printDebugMsg(DC_ERROR, errorLevel, "프로그램을 종료합니다.");
		system("pause");
		exit(1);
		break;
	}

	system("pause");
	return;
}