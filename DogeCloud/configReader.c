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
	@file configReader.c
	@date 2018/11/21
	@author 멍멍아야옹해봐
	@brief DogeCloud 설정파일 관련 함수 모음
*/

#include "DogeCloud.h"

/**
	@fn void checkDogeConfig()
	@brief DogeCloud 중계 서버 설정파일 유무 확인
	@author 멍멍아야옹해봐
*/
void checkDogeConfig() {
	FILE *fp;

	if (fopen_s(&fp, "./DogeConfig.ini", "r")) {//파일 있는지 검사
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "DogeConfig 파일이 없습니다.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "파일을 생성 해 주세요.");
		printDebugMsg(DC_ERROR, DC_ERRORLEVEL, "프로그램을 종료합니다.");
		system("pause");
		exit(1);//프로그램 에러코드 발생 및 종료
	}
	fclose(fp);//파일 닫기
	return;
}

/**
	@fn void readRelayServerPath(SOCKADDR_IN *RelayServAddr)
	@brief DogeCloud 중계 서버 설정파일 유무 확인
	@author 멍멍아야옹해봐
	@param *RelayServAddr 중계 서버 연결용 주소 구조체 주소
*/
void readRelayServerPath(SOCKADDR_IN *RelayServAddr) {
	checkDogeConfig();//설정파일 확인
	if (RelayServAddr == NULL) return;//주소 없으면 종료

	//받아온 구조체 0으로 초기화
	memset(RelayServAddr, 0, sizeof(SOCKADDR_IN));

	//주소 설정
	char tmpAddr[16] = { 0, };//주소 저장용 배열
	RelayServAddr->sin_family = AF_INET;
	GetPrivateProfileString("RelayServer", "ip", "127.0.0.1", tmpAddr, 15, "./DogeConfig.ini");//ini에서 읽어옴
	inet_pton(AF_INET, tmpAddr, &RelayServAddr->sin_addr.s_addr);//주소 변환 후 저장

	//포트 설정
	RelayServAddr->sin_port = htons(GetPrivateProfileInt("RelayServer", "port", 15384, "./DogeConfig.ini"));
	return;
}